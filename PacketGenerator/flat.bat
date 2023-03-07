erase /Q "../TempServer/fbb/"
erase /Q "../TempClient/fbb/"

(for %%a in (fbs/*.fbs) do ( 
flatc.exe --cpp --scoped-enums --gen-object-api --natural-utf8 -b -o ../TempServer/fbb fbs/%%a
flatc.exe --cpp --scoped-enums --gen-object-api --natural-utf8 -b -o ../TempClient/fbb fbs/%%a
))
pause


