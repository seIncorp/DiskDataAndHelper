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


- To get layout of HDD:

```DiskDataAndHelper.exe -Dl D -lay``` or ```DiskDataAndHelper.exe -Dp PhysicalDrive0 -lay``` (because D is on PhysicalDrive0 is the same result)

- To get performance of HDD:

```DiskDataAndHelper.exe -Dp PhysicalDrive0 -perf``` or ```DiskDataAndHelper.exe -Dl D -perf``` (because D is on PhysicalDrive0 is the same result)

- To get real disk data of logical disk:

```DiskDataAndHelper.exe -Dl D -realD```

- To get S.M.A.R.T data:

```DiskDataAndHelper.exe -Dp PhysicalDrive0 -predFailure``` or  ```DiskDataAndHelper.exe -Dl D -predFailure``` (because D is on PhysicalDrive0 is the same result)

- Device Configuration Overlay (DCO) data from specific HDD:

```DiskDataAndHelper.exe -Dp PhysicalDrive0 -DCO``` or  ```DiskDataAndHelper.exe -Dl D -DCO``` (because D is on PhysicalDrive0 is the same result)

- To get realy all Identify data of particular HDD:

```DiskDataAndHelper.exe -Dp PhysicalDrive0 -ATA_IDD``` or ```DiskDataAndHelper.exe -Dl D -ATA_IDD``` (because D is on PhysicalDrive0 is the same result)

- To get HPA data:

```DiskDataAndHelper.exe -Dp PhysicalDrive0 -HPAdata``` or  ```DiskDataAndHelper.exe -Dl D -HPAdata``` (because D is on PhysicalDrive0 is the same result)



## TODOs
- GUI
- Additional data about Hard drives
- HPA set
- DCO set
- cloning HDD
- Free-fall Control feature if HDD support
- IDENTIFY HDD DEVICE data (retriving all possible data about specific HDD)
- for Linux
- for Mac
