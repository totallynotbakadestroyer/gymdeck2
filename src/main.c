#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include "ryzenadj.h"

#define NUM_PHYSICAL_CORES 4
#define NUM_SAMPLES 10

typedef enum {
  AGGRESSIVE,
  DEFAULT,
  MANUAL
}
UndervoltingStrategy;

typedef struct {
  int point;
  int value;
}
ManualPoint;

void set_unsafe_coper(uint32_t value,
  const char * lang, ryzen_access ryzenadj) {
  if (!ryzenadj) {
    if (strcmp(lang, "cz") == 0) {
      fprintf(stderr, "Nelze inicializovat RyzenAdj\n");
    } else if(strcmp(lang, "kz") == 0)
      fprintf(stderr,"RyzenAdj inicializaciyasi mumkin emes\n");
    } else {
      fprintf(stderr, "Unable to initialize RyzenAdj\n");
    }
    exit(EXIT_FAILURE);
  }

  if (set_coper(ryzenadj, value) != 0) {
    if (strcmp(lang, "cz") == 0) {
      printf("Chyba při nastavování hodnoty coper pro jádro %d\n", value);
      fprintf(stderr, "Chyba při nastavování nebezpečné hodnoty coper\n");
    } else if (strcmp(lang, "kz") == 0){
      printf("jadro ushin coper menin ornity katesi %d\n", value);
      fprintf(stderr, "jadro ushin coper menin ornity katesi\n");
    } else {
      printf("Error setting coper value for core %d\n", value);
      fprintf(stderr, "Error setting unsafe coper value\n");
    }
  }
}

uint32_t calculate_hex_value(int core, int value) {
  unsigned int core_shifted = (unsigned int) core * 0x100000;
  unsigned int magnitude = (unsigned int)(-value & 0xFFFFF);
  unsigned int combined_value = core_shifted + magnitude;

  return combined_value;
}

float get_cpu_usage(int core_id,const char * lang) {
  static long prev_idle[NUM_PHYSICAL_CORES * 2] = {
    0
  };
  static long prev_total[NUM_PHYSICAL_CORES * 2] = {
    0
  };

  char buffer[1024];
  FILE * fp = fopen("/proc/stat", "r");
  if (!fp) {
    if (strcmp(lang, "cz") == 0) {
      perror("Nepodařilo se otevřít /proc/stat");
    } else if(strcmp(lang, "kz") == 0){
      perror("ashu mumkin emes /proc/stat");
    } else {
      perror("Failed to open /proc/stat");
    }
    return 0.0;
  }

  int line_num = 0;
  while (fgets(buffer, sizeof(buffer), fp)) {
    if (strncmp(buffer, "cpu", 3) == 0 && line_num == core_id + 1) {
      long user, nice, system, idle, iowait, irq, softirq, steal;
      sscanf(buffer, "cpu%*d %ld %ld %ld %ld %ld %ld %ld %ld", &
        user, & nice, & system, & idle, & iowait, & irq, & softirq, & steal);

      long idle_time = idle + iowait;
      long non_idle_time = user + nice + system + irq + softirq + steal;
      long total_time = idle_time + non_idle_time;

      long total_diff = total_time - prev_total[core_id];
      long idle_diff = idle_time - prev_idle[core_id];

      prev_total[core_id] = total_time;
      prev_idle[core_id] = idle_time;

      fclose(fp);

      if (total_diff == 0) {
        return 0.0;
      }

      return (float)(total_diff - idle_diff) / total_diff * 100.0;
    }
    line_num++;
  }

  fclose(fp);
  return 0.0;
}

void print_logo() {
    printf(" ________      ___    ___ _____ ______   ________  _______   ________  ___  __      _______     \n");
    printf("|\\   ____\\    |\\  \\  /  /|\\   _ \\  _   \\|\\   ___ \\|\\  ___ \\ |\\   ____\\|\\  \\|\\  \\   /  ___  \\    \n");
    printf("\\ \\  \\___|    \\ \\  \\/  / | \\  \\\\\\__\\ \\  \\ \\  \\_|\\ \\ \\   __/|\\ \\  \\___|\\ \\  \\/  /|_/__/|_/  /|   \n");
    printf(" \\ \\  \\  ___   \\ \\    / / \\ \\  \\\\\\|__| \\  \\ \\  \\ \\\\ \\ \\  \\_|/_\\ \\  \\    \\ \\   ___  \\__|//  / /   \n");
    printf("  \\ \\  \\|\\  \\   \\/  /  /   \\ \\  \\    \\ \\  \\ \\  \\_\\\\ \\ \\  \\_|\\ \\ \\  \\____\\ \\  \\\\ \\  \\  /  /_/__  \n");
    printf("   \\ \\_______\\__/  / /      \\ \\__\\    \\ \\__\\ \\_______\\ \\_______\\ \\_______\\ \\__\\\\ \\__\\|\\________\\\n");
    printf("    \\|_______|\\___/ /        \\|__|     \\|__|\\|_______|\\|_______|\\|_______|\\|__| \\|__| \\|_______|\n");
    printf("             \\|___|/                                                                           \n");
}


void print_usage(const char * prog_name, const char * lang) {
  if (strcmp(lang, "cz") == 0) {
    fprintf(stderr, "Použití: %s jazyk strategie interval_vzorkování maximum_core_0 minimum_core_0 prah_core_0 ... prah_core_3 [manuální_body_core_0] ... [manuální_body_core_3]\n", prog_name);
    fprintf(stderr, "Jazyk: 'en' pro angličtinu, 'cz' pro češtinu\n");
    fprintf(stderr, "Strategie: agresivní, konzervativní, výchozí, manuální\n");
    fprintf(stderr, "Interval vzorkování: mikrosekundy\n");
    fprintf(stderr, "Manuální body (pouze pro manuální strategii): [{bod: int, hodnota: int}, ...]\n");
  } else if(strcmp(lang, "kz") == 0)){
    fprintf(stderr, "Qoldanyluy: %s strategiya tilinin irikteu aralygn maximum_core_0 minimum_core_0 threshold_core_0 ... threshold_core_3 [manual_body_core_0] ... [manual_body_core_3]\n", prog_name);
    fprintf(stderr, "Til: agilsin usin 'en', cex usin 'cz'\n");
    fprintf(stderr, "Strategiya: agressivti, konservativti, adepki, qolmen\n");
    fprintf(stderr, "Tandau aralygi: mikrosekund\n");
    fprintf(stderr, "Qolmen nukter (tek qolmen strategiya usin): [{point: int, value: int}, ...]\n");
  } else {
    fprintf(stderr, "Usage: %s language strategy sample_interval core_0_maximum_value core_0_minimum_value core_0_threshold ... core_3_threshold [manual_points_core_0] ... [manual_points_core_3]\n", prog_name);
    fprintf(stderr, "Language: 'en' for English, 'cz' for Czech\n");
    fprintf(stderr, "Strategy: aggressive, conservative, default, manual\n");
    fprintf(stderr, "Sample Interval: microseconds\n");
    fprintf(stderr, "Manual Points (only for manual strategy): [{point: int, value: int}, ...]\n");
  }
}

int parse_manual_points(const char * json_str, ManualPoint * manual_points, int max_points, const char * lang) {
  int point_count = 0;
  const char * ptr = json_str;

  while ( * ptr && point_count < max_points) {
    while ( * ptr && ( * ptr == ' ' || * ptr == '{' || * ptr == '}' || * ptr == '[' || * ptr == ']' || * ptr == ',')) {
      ptr++;
    }

    if ( * ptr == '\0') {
      break;
    }

    int point = -1, value = -1;
    if (sscanf(ptr, "point: %d, value: %d", & point, & value) == 2) {
      manual_points[point_count].point = point;
      manual_points[point_count].value = value;
      point_count++;
    } else {
      break;
    }

    ptr = strchr(ptr, '}');
    if (ptr) {
      ptr++;
    }
  }

  if (point_count >= max_points) {
    if(strcmp(lang, "cz") == 0) {
      fprintf(stderr, "Varování: Překročen maximální počet manuálních bodů, bude použito pouze %d bodů\n", max_points);
    } else if(strcmp(lang, "kz") == 0){
      fprintf(stderr, "Eskeertu: Qolmen berilgen nukterdin maksimaldn sann asnp ketti, tek %d ūuan paydalanylady\n", max_points);
    } else {
      fprintf(stderr, "Warning: Max manual points exceeded, only %d points will be used\n", max_points);
    }
  }

  return point_count;
}

int main(int argc, char * argv[]) {
  print_logo();
  usleep(1000000);
  ryzen_access ryzenadj = init_ryzenadj();
  if (argc < (NUM_PHYSICAL_CORES * 3 + 4)) {
    print_usage(argv[0], "en");
    exit(EXIT_FAILURE);
  }

  // language
  const char * lang = argv[1];
  if (strcmp(lang, "cz") != 0 && strcmp(lang, "en") != 0 && strcmp(lang, "kz" != 0)) {
    fprintf(stderr, "Invalid language selection. Use 'en' for English or 'cz' for Czech.\n");
    exit(EXIT_FAILURE);
  }

  // strategy
  UndervoltingStrategy strategy;
  if (strcmp(argv[2], "aggressive") == 0) {
    strategy = AGGRESSIVE;
  } else if (strcmp(argv[2], "default") == 0) {
    strategy = DEFAULT;
  } else if (strcmp(argv[2], "manual") == 0) {
    strategy = MANUAL;
  } else {
    print_usage(argv[0], lang);
    exit(EXIT_FAILURE);
  }

  // sample interval
  int sample_interval = atoi(argv[3]);
  if (sample_interval <= 0) {
    if (strcmp(lang, "cz") == 0) {
      fprintf(stderr, "Neplatný interval vzorkování: %d\n", sample_interval);
    } else if(strcmp(lang, "kz") == 0){
      fprintf(stderr, "Jaramsyz irikteu aralygn: %d\n", sample_interval);
    } else {
      fprintf(stderr, "Invalid sample interval: %d\n", sample_interval);
    }
    exit(EXIT_FAILURE);
  }

  int max_curve_optimizer_steps[NUM_PHYSICAL_CORES];
  int min_curve_optimizer_steps[NUM_PHYSICAL_CORES];
  float thresholds[NUM_PHYSICAL_CORES];

  for (int i = 0; i < NUM_PHYSICAL_CORES; i++) {
    max_curve_optimizer_steps[i] = atoi(argv[4 + i * 3]);
    min_curve_optimizer_steps[i] = atoi(argv[5 + i * 3]);
    thresholds[i] = atof(argv[6 + i * 3]);

    if (max_curve_optimizer_steps[i] < 0 || min_curve_optimizer_steps[i] < 0 || thresholds[i] < 0.0 || thresholds[i] > 100.0) {
      if (strcmp(lang, "cz") == 0) {
        fprintf(stderr, "Neplatné vstupní hodnoty\n");
      } else if (strcmp(lang, "kz") == 0){
        fprintf(stderr, "Jaramsyz kiris manderi\n");
      } else {
        fprintf(stderr, "Invalid input values\n");
      }
      exit(EXIT_FAILURE);
    }
  }

  ManualPoint manual_points[NUM_PHYSICAL_CORES][10];
  int manual_point_count[NUM_PHYSICAL_CORES] = {
    0
  };

  if (strategy == MANUAL) {
    if (argc < (NUM_PHYSICAL_CORES * 3 + 4 + NUM_PHYSICAL_CORES)) {
      print_usage(argv[0], lang);
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < NUM_PHYSICAL_CORES; i++) {
      manual_point_count[i] = parse_manual_points(argv[(NUM_PHYSICAL_CORES * 3 + 4) + i], manual_points[i], 10, lang);
    }
  }

  float core_loads[NUM_PHYSICAL_CORES][NUM_SAMPLES] = {
    0
  };
  int sample_count[NUM_PHYSICAL_CORES] = {
    0
  };

  while (1) {
    for (int i = 0; i < NUM_PHYSICAL_CORES; i++) {
      float load1 = get_cpu_usage(i * 2, lang);
      float load2 = get_cpu_usage(i * 2 + 1, lang);
      float combined_load = (load1 + load2) / 2.0;

      core_loads[i][sample_count[i] % NUM_SAMPLES] = combined_load;
      sample_count[i]++;
    }

    system("clear");

    for (int i = 0; i < NUM_PHYSICAL_CORES; i++) {
      int samples_to_consider = sample_count[i] < NUM_SAMPLES ? sample_count[i] : NUM_SAMPLES;
      float total_load = 0.0;
      for (int j = 0; j < samples_to_consider; j++) {
        total_load += core_loads[i][j];
      }
      float average_load = total_load / samples_to_consider;

      int curve_optimizer_step = min_curve_optimizer_steps[i];

      switch (strategy) {
      case AGGRESSIVE:
        if (average_load > thresholds[i]) {
          float load_above_threshold = average_load - thresholds[i];
          float load_range = 100.0 - thresholds[i];
          float normalized_load = load_above_threshold / load_range;
          curve_optimizer_step = min_curve_optimizer_steps[i] + (int)(normalized_load * (max_curve_optimizer_steps[i] - min_curve_optimizer_steps[i]));
        } else {
          curve_optimizer_step = min_curve_optimizer_steps[i];
        }
        break;
      case DEFAULT:
        if (average_load > thresholds[i]) {
          curve_optimizer_step = (int)((average_load / 100.0) * max_curve_optimizer_steps[i]);
        } else {
          curve_optimizer_step = (int)((average_load / 100.0) * min_curve_optimizer_steps[i]);
        }
        if (curve_optimizer_step < min_curve_optimizer_steps[i]) {
          curve_optimizer_step = min_curve_optimizer_steps[i];
        }
        if (curve_optimizer_step > max_curve_optimizer_steps[i]) {
          curve_optimizer_step = max_curve_optimizer_steps[i];
        }
        break;
      case MANUAL:
        curve_optimizer_step = manual_points[i][0].value;
        for (int k = 0; k < manual_point_count[i]; k++) {
          if (average_load >= manual_points[i][k].point) {
            curve_optimizer_step = manual_points[i][k].value;
          } else {
            break;
          }
        }
      }
      int coper_value = calculate_hex_value(i, curve_optimizer_step);
      set_unsafe_coper((uint32_t) coper_value, lang, ryzenadj);

      if (strcmp(lang, "cz") == 0) {
        printf("Fyzické jádro %d: Průměrné zatížení: %.2f%%, Krok optimalizátoru křivky: %d, Coper hodnota: %d\n",
          i + 1, average_load, curve_optimizer_step, coper_value);
      } else if(strcmp(lang, "kz") == 0){
        printf("Fizikalyq negizgi %d: Ortasha jukte: %.2f%%, Qisyq oqtaylandyry qadamn: %d, Mis máni: %d\n",
          i + 1, average_load, curve_optimizer_step, coper_value);
      } 
      else {
        printf("Physical Core %d: Average Load: %.2f%%, Curve Optimizer Step: %d, Coper Value: %d\n",
          i + 1, average_load, curve_optimizer_step, coper_value);
      }
    }

    usleep(sample_interval);
  }

  cleanup_ryzenadj(ryzenadj);
  return 0;
}