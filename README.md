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

## Effect on CPU temperature

Some example cpu temperature measurements (making sure to give it plenty of time to settle
after a change) taken on my BeagleBoard-X15 while idle:
* 61 ͏°C if the requested power state is ON
* 55 ͏°C if the requested power state is INACTIVE
* 51 ͏°C if the requested power state is RETENTION
