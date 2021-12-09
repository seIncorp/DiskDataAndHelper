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

## TODOs
- GUI
- Additional data about Hard drives
- DCO
- HPA
- cloning HDD
- Free-fall Control feature if HDD support
- for Linux
- for Mac
