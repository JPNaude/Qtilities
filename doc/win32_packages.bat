doxygen doxyfile_packages
copy /y images\tab_*.png output\html_packages
copy /y images\bc_s.png output\html_website
copy /y sources\qtilities.qhcp output\html_packages
cd output
cd html_packages
C:\Tools\Qt\2010.03\qt\bin\qcollectiongenerator qtilities.qhcp -o qtilities.qhc
cd ..
cd ..
