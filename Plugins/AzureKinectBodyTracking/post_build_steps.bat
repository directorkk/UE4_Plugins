SET source_kinect_dll_path=%~dp0%ThirdParty\kinect-1.3.0\bin\x64\
SET editor_dll_path=%~dp0%Binaries\Win64\
SET package_dll_path=%~dp0%..\..\Content\DLLs

copy %source_kinect_dll_path%* %editor_dll_path%
copy %source_kinect_dll_path%* %package_dll_path%