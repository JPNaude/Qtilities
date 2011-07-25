doxygen doxyfile_packages
copy /y sources\qtilities.qhcp output\html_packages
cd output
cd html_packages
C:\Tools\Qt\QtSDK1.1\Desktop\Qt\4.7.2\mingw\bin\qcollectiongenerator qtilities.qhcp -o qtilities.qhc
cd ..
cd ..
