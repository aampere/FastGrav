# FastGrav
Forward modeling of microgravity anomaly profiles. Create and edit 2D/3D density models and forward-model their microgravity profiles.

## Background
The magnitude of gravity (~9.81m/s<sup>2</sup>) actually varies minutely from location to location, and the structure and density of geological masses under the ground have significant impact on these variations. FastGrav allows the user to construct a 2D or 3D model and calculate and plot variations in gravity at the ground surface. Here is one resource for reading more about microgravity anomalies: https://www.intechopen.com/chapters/57238

## Website
Visit [FastGrav.com](http://www.fastgrav.com) to download for Mac or PC, view user manual, demonstration videos, etc. Some of the info on the site is reproduced below.

## Download
To download a Windows build of FastGrav, visit the [Downloads](http://fastgrav.com/download/) page. The download pack also includes a detailed user manual, example projects, and example explanations. You can also download a Mac build, but it has not been built in some time may be out of date at this point. (See notes on Downloads page.)

## Demonstration Videos
These videos are the quickest way to see FastGrav at work. Geophysicists will get a sense of FastGrav's features and workflow, and programmers will get a sense of the program's parts. 
#### Gravity Modeling with FastGrav: Getting Started
<a href="https://www.youtube.com/watch?v=NA7Jt8ph8JA" target="_blank"><img src=https://user-images.githubusercontent.com/66134580/136272038-246df0ba-d873-4247-81b8-3b9e46e7146c.JPG width="400"></a>
#### FastGrav: Cross Sections and Profile Graphs
<a href="https://www.youtube.com/watch?v=Wm9-iAbIwdc" target="_blank"><img src=https://user-images.githubusercontent.com/66134580/136272101-02681723-077b-418f-bcbc-0cc3711faa5b.JPG width="400"></a>
#### FastGrav: 2D Modeling with Polygons
<a href="https://www.youtube.com/watch?v=ypOs95JBpM0" target="_blank"><img src=https://user-images.githubusercontent.com/66134580/136272195-c97b6ab7-365b-4c45-8d77-ba49f45065da.JPG width="400"></a>

## User Manual and Examples
See the [Downloads](http://fastgrav.com/download/) page to download a .zip which includes the application, the user manual, example projects, and example explanations.

## Build Instructions
FastGrav requires Qt to build. Qt is a cross-platform framework built on C++ that provides a great UI library. 
- Create a Qt account. https://login.qt.io/register You may need to describe what you're using Qt for. It is possible to obtain a non-commercial Qt license for free in this process, which is what I use.
- Install Qt: https://doc.qt.io/qt-5/gettingstarted.html.
  - I recommend installing Qt Creator, Qt's dedicated IDE, during this process, although other IDEs and workflows can work with configuration.
- Open `FastGrav.pro' in Qt Creator.
- Run.

## Use
FastGrav was developed completely by me, Aaron Price. Feel free to download, manipulate, or redistribute the code for academic or personal interest, for free use only. I am not currently doing development work on FastGrav--if you would like to move this project forward, please feel free to do so without further permission, or to [reach out](http://fastgrav.com/contact-2/) with questions. Please note that Qt is its own licensed framework, and that FastGrav was developed with a non-commercial Qt license, which you can also obtain to build this project yourself. If FastGrav is very useful in your research or your classroom, please consider [donating](http://fastgrav.com/donate/).
