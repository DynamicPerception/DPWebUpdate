DPWebUpdate
===========

The Dynamic Perception Web Updater allows for updating the firmware on any supported device on OSX and Windows.

Firmware updates are downloaded from a central repository, and updated on program start or by request.

See http://dynamicperception.com for more information.

Underlying Functionality
------------------------

Essentially, DPWebUpdate is a wrapper around avrdude.  A central UpdateIndex is used to maintain a repository of available updates, and firmware hex images are downloaded from this repository. The dpwebupdate.ini allows you to modify the repository location.  The repository is accessed via HTTP, and is indexed via a simple index.xml file.  The rest is handled automatically.

Copyright and License
---------------------

DPWebUpdate is Copyright &copy; 2013 Dynamic Perception LLC.

DPWebUpdate is shared under a GPLv3 License.

