

cl -AM -Zi -c  -Fc -Gs -G2 %1.c



if errorlevel 1 goto c_fail

link /ST:16384 /CO %1,,,graphics.lib+mylib.lib,,

:c_fail







