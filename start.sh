#!/bin/bash

# -------------------------------------
# Language Selection / Výběr jazyka
# -------------------------------------
# en = English, cz = Čeština
language="en"

# -------------------------------------
# Strategy Selection / Výběr strategie
# -------------------------------------
# Possible values / Možné hodnoty: 
# - aggressive
# - default
# - manual
strategy="default"

# -------------------------------------
# Sample Interval (in microseconds). In general, the lower the value, the less the risk of crashing the system, but the higher the CPU usage. Recommended value is 50000.
# Interval vzorkování (v mikrosekundách). Obecně platí, že čím nižší hodnota, tím menší riziko pádu systému, ale vyšší využití CPU. Doporučená hodnota je 50000.
# -------------------------------------
sample_interval=50000

# -------------------------------------
# Core 0 Configuration / Konfigurace jádra 0
# -------------------------------------
core_0_maximum_value=37
core_0_minimum_value=26
core_0_threshold=40.0

# -------------------------------------
# Core 1 Configuration / Konfigurace jádra 1
# -------------------------------------
core_1_maximum_value=37
core_1_minimum_value=26
core_1_threshold=40.0

# -------------------------------------
# Core 2 Configuration / Konfigurace jádra 2
# -------------------------------------
core_2_maximum_value=35
core_2_minimum_value=26
core_2_threshold=40.0

# -------------------------------------
# Core 3 Configuration / Konfigurace jádra 3
# -------------------------------------
core_3_maximum_value=36
core_3_minimum_value=26
core_3_threshold=40.0


# -------------------------------------
# Manual Points / Manuální body
# You can specify the points for the manual strategy here. They won't be used if the strategy is not set to manual. Add as many points as you want. /
# Zde můžete specifikovat body pro manuální strategii. Nebudou použity, pokud strategie není nastavena na manuální. Přidejte tolik bodů, kolik chcete.
# -------------------------------------
core_0_manual_points="[{point: 0, value: 25}, {point: 20, value: 30}, {point: 30, value: 30}]"
core_1_manual_points="[{point: 0, value: 25}, {point: 15, value: 25}, {point: 40, value: 30}]"
core_2_manual_points="[{point: 10, value: 20}, {point: 50, value: 30}]"
core_3_manual_points="[{point: 25, value: 25}, {point: 35, value: 30}]"

# -------------------------------------
# Check if libryzenadj.so is in /usr/lib or standard system paths
# -------------------------------------
if ! ldconfig -p | grep -q libryzenadj.so; then
    export LD_LIBRARY_PATH=./lib/:$LD_LIBRARY_PATH
fi

# Run the application / Spuštění aplikace
# Make sure you're refering to the correct path of the application. /
# Ujistěte se, že se odkazujete na správnou cestu aplikace.
./gymdeck2 "$language" "$strategy" "$sample_interval" \
    "$core_0_maximum_value" "$core_0_minimum_value" "$core_0_threshold" \
    "$core_1_maximum_value" "$core_1_minimum_value" "$core_1_threshold" \
    "$core_2_maximum_value" "$core_2_minimum_value" "$core_2_threshold" \
    "$core_3_maximum_value" "$core_3_minimum_value" "$core_3_threshold" \
    "$core_0_manual_points" "$core_1_manual_points" \
    "$core_2_manual_points" "$core_3_manual_points"
