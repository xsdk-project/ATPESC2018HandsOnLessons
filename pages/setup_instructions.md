---
layout: page
show_meta: false
title: "Setup Instructions"
subheadline: "Instructions to complete prior to August 6th."
header:
   image_fullwidth: "atpesc-1024x2746.jpg"
permalink: "/setup_instructions/"
---

Instructions here are divided into _required_ and _optional_ steps.
We expect everyone to, minimally, completed all _required_ steps here.
The _optional_ steps are likely to improve your experience by simplifying
or improving performance of certain operations.

## Required Steps

Please complete the followig _required_ steps prior to the beginning of the session
on Monday, August 6th.

1. Log Into Cooley
  * Use secure shell with compression, and trusted X forwarding enabled
```
ssh -l <username> -C -Y cooley.alcf.anl.gov
```
1. Copy Installed Software
* Once you are logged into Cooley, please execute the following instruction
to create a local, editable copy of numerical package software.
```
cd ~
rsync -a {{site.handson_install_root}}/{{site.handson_root}} .
```
  * **Note 1:** do not include a trailing slash, `/` in the path argument.
  * **Note 2:** You may be asked periodically throughout the day to re-execute
this command to update your local copy if we discover changes are necessary.
1. Confirm you can compile and run an example
  * As a test case, use an example from hypre to confirm you can compile
    and run an example
```
qsub -I -n 1 -t 5 -A ATPESC2018
cd HandsOnLessons/hypre
touch ij.c
make ij
mpirun -np 4 ./ij
echo $?
exit
``` 
  * The `qsub` command reserves a cooley node for interactive work for 5 minutes.
    You may have to wait a moment for the interactive prompt on the reserved node to return.
  * The above commands should produce makefile and execution output. In particular
    the last `echo` command should produce a `0` response.
1. As soon after 9am, Monday, August 6th as possible, allocate an interactive node on
   cooley. The following command allocates a single Cooley node (`-n 1`) for 480 minutes
   (`-t 480`) using the ATPESC2018 allocation (`-A ATPESC2018`) and the training reservation (`-q training`):
```
qsub -I -n 1 -t 480 -A ATPESC2018 -q training
```
The command blocks until the node is ready.  Until the allocation expires (480mins in this example), all commands executed in the returned session will run on the allocated compute node; `mpirun` can be used directly instead of going through `qsub`.
  * **Note 1:** The special `-q training` will not be functional until 9am, August 6th and will go away 9pm that same day.
  * **Note 2:** All the software here can also run on the login nodes of Cooley. However, we ask that you please
    **DO NOT** run jobs on the login nodes and instead run them on a reserved allocation.
  * **Note 3:** Be aware, however, that starting new jobs on the reserved node too close to the allocation's expiration
    may result in the allocation going away before your job completes.

## Visualization Tools

There are three options



**Note**: Client-Server is Best Option

#### Installing and Setting up VisIt for Client-Server to Cooley

#### Installing and Setting up ParaView for Client-Server to Cooley

**Can Cameron write this section?**


### Running Remotely via X-Windows

**Note**: Requires X-Server with indirect GLX enabled.

### VisIt

#### Client-Server vs. All Local

**Note**: add `@visit` to `~/.soft.cooley` and run `resoft` on Cooley login node

### ParaView

### GlVis

## Transferring Data

### Secure Shell Control Master

### Secure File Transfer Protocol (sftp)




currently, just notes to self

* confirm cooley login
* confirm display via x back to laptop
* confirm editor(s) available (vi, nano, ...)
* confirm run apps in /projects/ATPESC2018
* confirm run VisIt
  * install instructs for local VisIt
* confirm run ParaView
  * install instructs for local ParaView (don't know this)
* confirm run glvis
* confirm run gnuplot
* windows, osx and linux specific setup
* vnc setup and/or script?
* run a script (we still need to write) to set paths, etc.
* instructions on reserving a node (w/ Cobalt) If request interactive, get it immediately

* As a back up, confirm all above on NERSC/Cori
  * perhaps this is too much to ask?

ALCF Rerservation Info

#### Reservation: ATPESC2018_0806_1

* Queue       : training
* User        : None
* Start       : Mon Aug 06 09:00:00 2018 -0500 (CDT)
* Duration    : 12:00
* End Time    : Mon Aug 06 21:00:00 2018 -0500 (CDT)
* Partitions  : [cc038-039],[cc043-125]
