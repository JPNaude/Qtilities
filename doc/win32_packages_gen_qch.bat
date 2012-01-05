copy /y sources\qtilities.qhcp output\html_packages
cd output
cd html_packages
D:\Tools\QtSDK\Desktop\Qt\4.7.4\mingw\bin\qcollectiongenerator qtilities.qhcp -o qtilities.qhc
copy /y qtilities.qch ..\..\Qtilities.qch
cd ..
cd ..
