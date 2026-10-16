# Zephyr Testing Sample

How to set up, build and run this repository.

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

### Run on `native_sim`

Build the application:

```shell
west build -p always -b native_sim app
```

Run the application:

```shell
west build -t run
```

Expected output:

```
*** Booting Zephyr OS build v4.5.0-rc1 ***
Hello World!
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

Expected output:

```
*** Booting Zephyr OS build v4.5.0-rc1 ***
Hello World!
```

Press `Ctrl+A`, release, then press `X` to exit QEMU.

### Seven

Build the application:

```shell
west build -p always -b seven/nrf9151 app
```

Flash it to the board:

```shell
west flash
```