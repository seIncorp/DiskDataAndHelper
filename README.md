# DiskDataAndHelper
To get all data about Hard driver and more...

With this app, you can see all about used hard drivers also hidden data that Windows does not show. 

**__Currently this app is for Windows__**

## CMD parameters

-H  = this is for printing help

### Setting which type will be used with the name of it
-Dp "name"
> with this you can get data for physical disk, instead "name" you write for example: PhysicalDrive0

-Dl "name"
> with this you can get data for logical disk, instead "name" you write for example: C or D or ...

-All
> with this you will get all data for all logical disks

### Additional parameters
-att
> with this you will get disk attributes

-geo
> with this you will get Drive geometry

-lay
> with this you will get Drive layout

-perf
> with this you will get Disk performance

-realD
> with this you will get real disk (it is uesd only for logical disk)

-predFailure
> this command return all S.M.A.R.T data from HDD

-DCO
> to get all Device Configuration Overlay (DCO) data from specific HDD direct from HDD without controler

-ATA_IDD (NOT finished yet!!)
> to get all data of IDENTIFY device direct from HDD without controler

-HPAdata
> to get HPA data direct from HDD without controler


## Examples:

- Selecting all partitions (you get only letters of all partitions):

```DiskDataAndHelper.exe -All```

- Select physical disk for getting data from it:

```DiskDataAndHelper.exe -Dp PhysicalDrive0```

- Select logical partition:

```DiskDataAndHelper.exe -Dl D```

- To get all disk attributes:

```DiskDataAndHelper.exe -Dp PhysicalDrive0 -att``` or ```DiskDataAndHelper.exe -Dl D -att``` (because D is on PhysicalDrive0 is the same result)

- To get geometry disk:

```DiskDataAndHelper.exe -Dp PhysicalDrive0 -geo``` or ```DiskDataAndHelper.exe -Dl D -geo``` (because D is on PhysicalDrive0 is the same result)


## TODOs
- GUI
- Additional data about Hard drives
- HPA
- cloning HDD
- Free-fall Control feature if HDD support
- IDENTIFY HDD DEVICE data (retriving all possible data about specific HDD)
- for Linux
- for Mac
