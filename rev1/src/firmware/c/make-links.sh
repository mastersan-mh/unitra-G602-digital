list="config.hpp
defs.hpp
utils.hpp
unitra-g602.ino
G602.hpp
G602.ino
GBlinkerPatterned.hpp
GBlinkerPatterned.ino
GBlinker.hpp
GBlinker.ino
Ctrl.hpp
Ctrl.ino
GTime.hpp
GObject.hpp
GDInput.hpp
GDInput.ino
GDInputDebounced.hpp
GDInputDebounced.ino
GCommBase.hpp
GCommBase.ino
GComm.hpp
GComm.ino
GRPCDefs.hpp
GRPCServer.hpp
GRPCServer.ino
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

