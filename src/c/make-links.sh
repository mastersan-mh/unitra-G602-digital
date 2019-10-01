list="config.hpp
defs.hpp
unitra-g602.ino
GBlinkerPatterned.hpp
GBlinkerPatterned.ino
GBlinker.hpp
GBlinker.ino
Ctrl.hpp
Ctrl.ino
GTime.hpp
GObject.hpp
GObject.ino
GDInput.hpp
GDInput.ino
GDInputDebounced.hpp
GDInputDebounced.ino
AverageTinyMemory.hpp
AverageTinyMemory.ino
"


for src in ${list}
do
    if ( echo "${src}" | grep ".ino$" > /dev/null ) then
        dest=$( echo "${src}" | sed "s/\.ino$/\.cpp/g" )
    else
        dest=${src}
    fi
    echo "processing: ${src} -> ${dest}"
    ln -s ../ino/unitra-g602/${src} ./${dest}
done

