# omap5-cpu-pm
Utility for displaying/changing OMAP5/AM57xx CPU power management registers

CAUTION: Use at your own risk! I don't know why CPU power management doesn't seem to be
enabled by default, but perhaps there's actually a good reason for it.  Don't blame me if
using this utility causes spontaneous combustion.

## Installation

```bash
make && sudo cp omap5-cpu-pm /usr/local/sbin/
```

## Usage

To display current settings and state:
```bash
sudo omap5-cpu-pm
```

To change the power domain state requested for each core when idle:
```bash
sudo omap5-cpu-pm --on          # remain in ON state at all times
sudo omap5-cpu-pm --inactive    # enter INACTIVE state when cpu idle
sudo omap5-cpu-pm --retention   # enter RETENTION state when cpu idle
```

In both INACTIVE and RETENTION states the cpu clock is disabled.
In RETENTION state the cpu voltage is also lowered.

Note that lower power states will increase wake-up latency.

## Issues

Enabling cpu power management appears to mess with cpu load measurement, causing
falsely high system load to be reported by `uptime`. This also appears to cause the
"ondemand" cpufreq governor to keep the cpu frequency unnecessarily high.  On the
other hand, enabling cpu power management appears to be more effective in reducing
power dissipation than reducing the cpu frequency (see below).

## Effect on CPU temperature

Some example cpu temperature measurements (making sure to give it plenty of time to settle
after a change) taken on my BeagleBoard-X15 while idle:
* 61 ͏°C with cpufreq governor "performance" (1.5 GHz) and requested power state ON
* 56 ͏°C with cpufreq governor "powersave" (1.0 GHz) and requested power state ON
* 55 ͏°C with cpufreq governor "performance" (1.5 GHz) and requested power state INACTIVE
* 51 ͏°C with cpufreq governor "performance" (1.5 GHz) and requested power state RETENTION
