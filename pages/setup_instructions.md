---
layout: page
show_meta: false
title: "Setup Instructions"
subheadline: "Instructions to complete prior to August 6th."
header:
   image_fullwidth: "atpesc-1024x2746.jpg"
permalink: "/setup_instructions/"
---

## Seting Up Soft Enviornment on Cooley

## Allocating an Interactive session on Cooley

The following command allocates a single Cooley node (`-n 1`) for 30 minutes (`-t 30`) using the ATPESC2018 allocation (`-A ATPESC2018`) and the training reservation (`-q training`):

```
qsub -I -n 1 -t 30 -A ATPESC2018
```

The command blocks until the node is ready.  Until the allocation expires (30mins in this example), all commands executed in the returned session will run on the allocated compute node; `mpirun` can be used directly instead of going through `qsub`.

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
