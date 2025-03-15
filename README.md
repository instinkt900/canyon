## Setup
```
python3 -m venv .venv
source .venv/bin/activate
pip install conan
```

## Build
```
conan install . --profile linux_profile --build missing -s build_type=Debug
cmake --preset conan-debug
cmake --build --preset conan-debug
```

