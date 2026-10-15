# Zephyr Testing Sample

How to set up, build and run this repository.

## Prerequisites

Follow the official
[Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
to install the host dependencies and the Zephyr SDK. On Linux, the SDK also
includes QEMU.

You do not need a separate `~/zephyrproject` workspace. The steps below
create a workspace with the Zephyr revision pinned in `west.yml`.

## Initializing the workspace

```shell
mkdir ~/zts-workspace && cd ~/zts-workspace
python3 -m venv .venv
source .venv/bin/activate
pip install west

west init -m git@github.com:id8-engineering/zephyr-testing-sample.git
west update

pip install -r zephyr/scripts/requirements.txt
west zephyr-export
```

In every new terminal, activate the virtual environment and go to the
repository:

```shell
source ~/zts-workspace/.venv/bin/activate
cd ~/zts-workspace/zephyr-testing-sample
```

After pulling changes to `west.yml`, run `west update` again.

## Building and running

Run all commands from `~/zts-workspace/zephyr-testing-sample`.

```shell
west build -p always -b <board> app
```

### native_sim

```shell
west build -p always -b native_sim app
west build -t run
```

Expected output:

```
*** Booting Zephyr OS build v4.5.0-rc1 ***
Hello World!
```

Press `Ctrl+C` to stop.

### qemu_cortex_m3

```shell
west build -p always -b qemu_cortex_m3 app
west build -t run
```

Expected output:

```
*** Booting Zephyr OS build v4.5.0-rc1 ***
Hello World!
```

Press `Ctrl+A`, release, then press `X` to exit QEMU.

### Physical hardware

```shell
west build -p always -b <board> app
west flash
```

Read the output on the board's serial console, for example:

```shell
minicom -D /dev/ttyACM0 -b 115200
```

Press `Ctrl+A`, then `X` to exit minicom.
