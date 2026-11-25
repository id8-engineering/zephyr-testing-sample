# Zephyr Testing Sample

How to set up, build and run this repository.

The application samples a sensor periodically, buffers the readings and
publishes them as JSON. See [docs/architecture.md](docs/architecture.md) for
how it is built.

## Prerequisites

Follow the official
[Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
to install the host dependencies and the Zephyr SDK. On Linux, the SDK also
includes QEMU.

You do not need a separate `~/zephyrproject` workspace. The steps below
create a workspace with the Zephyr revision pinned in `west.yml`.

## Initialize workspace

Create the workspace and change directory:

```shell
mkdir ~/zts-workspace && cd ~/zts-workspace
```

Create a Python virtual environment:

```shell
python3 -m venv .venv
```

Activate the virtual environment:

```shell
source .venv/bin/activate
```

Install west:

```shell
pip install west
```

Initialize workspace:

```shell
west init -m git@github.com:id8-engineering/zephyr-testing-sample.git
```

Fetch Zephyr and the modules listed in `west.yml`:

```shell
west update
```

Install Python dependencies for Zephyr:

```shell
pip install -r zephyr/scripts/requirements.txt
```

Export the Zephyr CMake package:

```shell
west zephyr-export
```

In every new terminal, activate the virtual environment:

```shell
source ~/zts-workspace/.venv/bin/activate
```

Go to the repository:

```shell
cd ~/zts-workspace/zephyr-testing-sample
```

### Update workspace

`west.yml` pins the versions of Zephyr and the modules this project uses.

> [!TIP]
> Run `west update` in case any module references were updated, for
> example after `git pull`.

## Build and run

Run all commands from `~/zts-workspace/zephyr-testing-sample`.

> ```shell
> west build -p always -b <board> app
> ```

### Change the sampling interval

The interval is set by `CONFIG_TELEMETRY_APP_SAMPLE_INTERVAL_MS`. To sample every
second, for example:

```shell
west build -p always -b native_sim app -- -DCONFIG_TELEMETRY_APP_SAMPLE_INTERVAL_MS=1000
```

### Run on `native_sim`

Build the application:

```shell
west build -p always -b native_sim app
```

Run the application:

```shell
west build -t run
```

Expected output, one record every 10 seconds:

```
*** Booting Zephyr OS build v4.5.0-1358-gfbe8947dd0a1 ***
[00:00:00.010,000] <inf> publisher_log: {"seq":0,"uptime_ms":10,"temperature_mc":21500,"humidity_mpct":45000}
[00:00:10.010,000] <inf> publisher_log: {"seq":1,"uptime_ms":10010,"temperature_mc":21500,"humidity_mpct":45000}
[00:00:20.010,000] <inf> publisher_log: {"seq":2,"uptime_ms":20010,"temperature_mc":21500,"humidity_mpct":45000}
```

Press `Ctrl+C` to stop.

### Run on `qemu_cortex_m3`

Build the application:

```shell
west build -p always -b qemu_cortex_m3 app
```

Run the application:

```shell
west build -t run
```

Expected output, one record every 10 seconds:

```
*** Booting Zephyr OS build v4.5.0-1358-gfbe8947dd0a1 ***
[00:00:00.010,000] <inf> publisher_log: {"seq":0,"uptime_ms":10,"temperature_mc":21500,"humidity_mpct":45000}
[00:00:10.010,000] <inf> publisher_log: {"seq":1,"uptime_ms":10010,"temperature_mc":21500,"humidity_mpct":45000}
[00:00:20.010,000] <inf> publisher_log: {"seq":2,"uptime_ms":20010,"temperature_mc":21500,"humidity_mpct":45000}
```

Press `Ctrl+A`, release, then press `X` to exit QEMU.

### Run on Seven

Build the application:

```shell
west build -p always -b seven/nrf9151 app
```

Flash it to the board:

```shell
west flash
```

## Run tests

Run all commands from `~/zts-workspace/zephyr-testing-sample`.

Run the unit tests on `native_sim`:

```shell
west twister -T tests --integration
```

Run the application on `native_sim` and `qemu_cortex_m3` and check its log
output:

```shell
west twister -T app --integration
```
Twister writes its output and logs to `twister-out/`. See
[Test output and reports](https://docs.zephyrproject.org/latest/develop/twister/index.html#test-output-and-reports)
in the Twister documentation for details.

## Code Formatting

All C code must follow the [Zephyr coding style](https://docs.zephyrproject.org/latest/contribute/style/code.html).

This project uses `pre-commit` to automatically format C code with `clang-format` before each commit.

Install `pre-commit` in your virtual environment:

```shell
pip install pre-commit
```

Enable the Git hook (once per clone):

```shell
pre-commit install
```

If formatting changes are made, the commit will be aborted. Stage the changes and commit again.

To manually format all tracked files:

```shell
pre-commit run --all-files
```

## Telemetry log format

The logger publisher encodes each telemetry record as a JSON object and writes it
to one log line. All values are integers using the field names and units below:

```
<inf> publisher_log: {"seq":0,"uptime_ms":0,"temperature_mc":21500,"humidity_mpct":45000}
```

| Field            | Type       | Unit                  | Description       | Example                |
|------------------|------------|-----------------------|-------------------|------------------------|
| `seq`            | `uint32_t` | -                     | Sequence number   | `0`                    |
| `uptime_ms`      | `int64_t`  | milliseconds          | System uptime     | `0` (0 ms)             |
| `temperature_mc` | `int32_t`  | milli degrees Celsius | Temperature       | `21500` (21.5 °C)      |
| `humidity_mpct`  | `uint32_t` | milli percent RH      | Relative humidity | `45000` (45.0 %RH)     |

Example:

```
[00:00:00.000,000] <inf> publisher_log: {"seq":0,"uptime_ms":0,"temperature_mc":21500,"humidity_mpct":45000}
```

Example JSON record:

```
{"seq":0,"uptime_ms":0,"temperature_mc":21500,"humidity_mpct":45000}
```
