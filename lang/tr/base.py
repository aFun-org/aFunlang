help_info = r'''[option]
   help
       -v Show    version info
       -h Show    help page
   run
       The first parameter is the running file
       The following parameters are command line parameters
   cl
       -e/--eval    Run argument as code
       -f/--file    Run file (.aun or .aub)
       -s/--source    Run source file
       -b/--byte    Run byte-code file
       --no-aub    Do not save byte-code after run source file
       --no-cl    Do not use command line
       The free parameter before -- will be interpreted as the -f parameter
       The free parameter after -- will be interpreted as the command line parameters
   build
       -o File output path
       -p File output dir
       -f Force build

[Github page]
   <https://github.com/aFun-org/aFunlang>'''