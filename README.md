# uftrace_plugin_nodejs
Uftrace(https://github.com/namhyung/uftrace) is the tool to trace and analyze execution of a program written in C/C++. it provide various commands and options to suppose benefit to user who want to trace any linux program with any reason. 

as a derivate project the Uftrace_plugin_nodejs have purpose to support user who want to trace javascript function calling in nodejs. because nodejs characteristic, some logic need to add to uftrace for trace javascript function calling. but it is no need when trace general linux program. so, feature  of javascript function call tracing in nodejs was developed as a plugin type.


# features
Uftrace_plugin_nodejs's features are work by hook which implmented in uftrace. as mention above, uftrace can record each function calling in linux program. Uftrace_plugin_nodejs receive the information that which function was called from uftrace. only three function is need to trace javascript function call. 
- v8::internal::Runtime_TraceEnter
- v8::internal::Runtime_TraceExit
- v8::internal::JavaScriptFrame::PrintTop

Uftrace_plugin_nodejs do some manipulation while this functions called. and record it to file has named 'extern.dat' in uftrace data path. also, if you use python script like 'replay.py' which located under scripts path in uftrace, then you can see the name of called javascript function in realtime.


# Usage

    $ uftrace record -F v8::internal::Execution::Call -F v8::internal::Runtime_TraceExit \
      -I/home/m/git/uftrace/plugins/plugin_nodejs.so ../node/node -trace simple_http.js

# dependencies 
uftrace_plugin_nodejs has dependencies for below libraries:
* `proto`
* `grpc`
* `uftrace`

# Build
    $ make 
    

# how it works
Uftrace_plugin_nodejs manipulate argument when v8::internal::JavaScriptFrame::PrintTop called. as the result of manipulated argument, v8::internal::JavaScriptFrame::PrintTop function pass the currently called javascript function name to uftrace_plugin_nodejs.


# License 
The uftrace program is released under GPL v2. See COPYING file for details.

# showcase
![alt text](https://github.com/ParkHanbum/uftrace_plugin_nodejs/blob/gh-pages/pinpoint.jpg)
