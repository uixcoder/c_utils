# 🐧 Linux (Debian/Ubuntu): Налаштування VS Code для розробки на C

## Виявлення витоків пам'яті: gcc, AddressSanitizer, Valgrind, GDB

---

## 🎯 Два підходи — який обрати?

```
Варіант А — Нативно (простіше):
  Linux → gcc / valgrind / gdb напряму
  ✅ Нуль накладних витрат
  ✅ Найпростіше налаштування
  ✅ Ідеально для навчання та одиночної розробки

Варіант Б — Docker (гнучкіше):
  Linux → Docker Engine → контейнер → gcc / valgrind / gdb
  ✅ Ізольоване середовище для кожного проекту
  ✅ Dockerfile у git = відтворюване середовище
  ✅ Кілька версій gcc паралельно
  ⚠️ Додатковий шар = більше налаштувань
```

### Порівняльна таблиця

| Критерій | **Варіант А: Нативно** | **Варіант Б: Docker** |
|---|---|---|
| Складність налаштування | ✅ Мінімальна | ⚠️ Середня |
| Швидкість компіляції | ✅ Максимальна | ✅ Майже однакова |
| GCC / Clang / Valgrind / ASan | ✅ | ✅ |
| GDB дебагер | ✅ | ✅ (потребує `--cap-add`) |
| Ізоляція середовища | ❌ | ✅ |
| Відтворюваність (git) | ❌ | ✅ Dockerfile |
| Кілька версій gcc | ❌ Складно | ✅ Легко |
| Командна розробка | ⚠️ | ✅ |
| Зайняте місце | ~300 MB | ~700 MB |

### Рекомендація

```
Навчання / pet-проекти  →  Варіант А (нативно)
Командна робота         →  Варіант Б (Docker)
Кілька проектів з різними залежностями  →  Варіант Б
```

---

## 📋 Вимоги

- Debian 11+ / Ubuntu 20.04+ або будь-який похідний дистрибутив
- `sudo` доступ
- ~1 GB вільного місця (Варіант А) або ~3 GB (Варіант Б)

---

---

# ▶ ВАРІАНТ А: Нативна розробка на Linux

## Архітектура

```
┌─────────────── Linux ──────────────────┐
│                                         │
│  VS Code                                │
│    └── C/C++ Extension                  │
│          │ запускає напряму             │
│          ▼                              │
│  gcc / g++ / gdb / valgrind / cppcheck  │
│                                         │
│  ~/C_Projects/my_project/  ← файли тут │
└─────────────────────────────────────────┘
```

---

## A.1 Встановлення інструментів розробки

```bash
sudo apt update

sudo apt install -y \
  build-essential \
  gcc \
  g++ \
  gdb \
  valgrind \
  cppcheck \
  clang \
  clang-tools \
  libasan8
```

> **Примітка для Debian 11 / Ubuntu 20.04:** пакет називається `libasan6` замість `libasan8`.
> Перевірте доступну версію: `apt-cache search libasan`

### Перевірка встановлення

```bash
gcc --version
valgrind --version
cppcheck --version
gdb --version | head -1
```

**Очікуваний вивід:**

```
gcc (Debian 12.2.0-14) 12.2.0
valgrind-3.19.0
Cppcheck 2.10
GNU gdb (Debian 13.1-3) 13.1
```

**Якщо всі чотири команди працюють — інструменти встановлені! ✅**

---

## A.2 Встановлення VS Code

### Спосіб 1: .deb пакет (рекомендовано)

```bash
# Завантажити та встановити
curl -fsSL https://packages.microsoft.com/keys/microsoft.asc \
  | gpg --dearmor \
  | sudo tee /etc/apt/keyrings/microsoft.gpg > /dev/null

echo "deb [arch=amd64 signed-by=/etc/apt/keyrings/microsoft.gpg] \
  https://packages.microsoft.com/repos/code stable main" \
  | sudo tee /etc/apt/sources.list.d/vscode.list

sudo apt update
sudo apt install -y code
```

### Спосіб 2: Snap

```bash
sudo snap install code --classic
```

### Спосіб 3: Ручне завантаження

Завантажте `.deb` з https://code.visualstudio.com/ та встановіть:

```bash
sudo dpkg -i code_*.deb
```

---

## A.3 Встановлення розширень VS Code

```bash
# C/C++ підтримка
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cpptools-extension-pack

# Підсвічування помилок прямо в коді
code --install-extension usernamehw.errorlens

# Hex редактор (для аналізу пам'яті)
code --install-extension ms-vscode.hexeditor
```

**Перевірка:**

```bash
code --list-extensions
```

---

## A.4 Створення проекту

```bash
mkdir -p ~/C_Projects/my_project/{.vscode,src}
cd ~/C_Projects/my_project
code .
```

### Структура проекту

```
~/C_Projects/my_project/
├── .vscode/
│   ├── tasks.json
│   ├── launch.json
│   ├── settings.json
│   └── c_cpp_properties.json
└── src/
    └── main.c
```

---

## A.5 Конфігураційні файли VS Code

### `.vscode/tasks.json`

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Build with AddressSanitizer",
      "type": "shell",
      "command": "gcc",
      "args": [
        "-g",
        "-O0",
        "-fsanitize=address",
        "-fsanitize=leak",
        "-fno-omit-frame-pointer",
        "-Wall",
        "-Wextra",
        "${file}",
        "-o",
        "${fileDirname}/${fileBasenameNoExtension}_asan"
      ],
      "group": {
        "kind": "build",
        "isDefault": true
      },
      "problemMatcher": ["$gcc"],
      "detail": "Компіляція з AddressSanitizer",
      "presentation": {
        "echo": true,
        "reveal": "always",
        "focus": false,
        "panel": "shared"
      }
    },
    {
      "label": "Run ASan (Terminal)",
      "type": "shell",
      "command": "${fileDirname}/${fileBasenameNoExtension}_asan",
      "dependsOn": ["Build with AddressSanitizer"],
      "problemMatcher": [],
      "presentation": {
        "reveal": "always",
        "panel": "new",
        "focus": true
      },
      "detail": "Запустити програму з AddressSanitizer"
    },
    {
      "label": "Build Debug",
      "type": "shell",
      "command": "gcc",
      "args": [
        "-g", "-O0", "-Wall", "-Wextra",
        "${file}",
        "-o", "${fileDirname}/${fileBasenameNoExtension}_debug"
      ],
      "group": "build",
      "problemMatcher": ["$gcc"],
      "detail": "Звичайна debug компіляція"
    },
    {
      "label": "Run Valgrind Memcheck",
      "type": "shell",
      "command": "bash",
      "args": [
        "-c",
        "gcc -g -O0 -Wall -Wextra \"${file}\" -o \"${fileDirname}/${fileBasenameNoExtension}_debug\" && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=\"${fileDirname}/valgrind-out.txt\" \"${fileDirname}/${fileBasenameNoExtension}_debug\""
      ],
      "problemMatcher": [],
      "presentation": {
        "reveal": "always",
        "panel": "shared"
      },
      "detail": "Компіляція та запуск Valgrind (результат у valgrind-out.txt)"
    },
    {
      "label": "Run Valgrind Quick",
      "type": "shell",
      "command": "bash",
      "args": [
        "-c",
        "gcc -g -O0 -Wall -Wextra \"${file}\" -o \"${fileDirname}/${fileBasenameNoExtension}_debug\" && valgrind --leak-check=yes \"${fileDirname}/${fileBasenameNoExtension}_debug\""
      ],
      "problemMatcher": [],
      "presentation": {
        "reveal": "always",
        "panel": "dedicated",
        "focus": true,
        "clear": true
      },
      "detail": "Компіляція та швидкий запуск Valgrind"
    },
    {
      "label": "Static Analysis - Cppcheck",
      "type": "shell",
      "command": "cppcheck",
      "args": [
        "--enable=warning,style,performance,portability",
        "--inconclusive",
        "--std=c11",
        "--suppress=missingIncludeSystem",
        "${file}"
      ],
      "problemMatcher": [],
      "presentation": {
        "reveal": "always",
        "panel": "dedicated",
        "focus": true,
        "clear": true
      },
      "detail": "Статичний аналіз з Cppcheck"
    },
    {
      "label": "Build and Run (Simple)",
      "type": "shell",
      "command": "bash",
      "args": [
        "-c",
        "gcc -g -O0 -Wall -Wextra \"${file}\" -o \"${fileDirname}/${fileBasenameNoExtension}\" && \"${fileDirname}/${fileBasenameNoExtension}\""
      ],
      "problemMatcher": ["$gcc"],
      "presentation": {
        "reveal": "always",
        "panel": "dedicated",
        "focus": true,
        "clear": true
      },
      "detail": "Компіляція та звичайний запуск"
    },
    {
      "label": "Clean Build Files",
      "type": "shell",
      "command": "bash",
      "args": [
        "-c",
        "rm -f \"${fileDirname}/${fileBasenameNoExtension}_asan\" \"${fileDirname}/${fileBasenameNoExtension}_debug\" \"${fileDirname}/valgrind-out.txt\" \"${fileDirname}\"/*.plist \"${fileDirname}/${fileBasenameNoExtension}\""
      ],
      "problemMatcher": [],
      "presentation": { "reveal": "silent" },
      "detail": "Видалити скомпільовані файли"
    }
  ]
}
```

### `.vscode/launch.json`

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug Standard",
      "type": "cppdbg",
      "request": "launch",
      "program": "${fileDirname}/${fileBasenameNoExtension}_debug",
      "args": [],
      "stopAtEntry": false,
      "cwd": "${fileDirname}",
      "environment": [],
      "externalConsole": false,
      "MIMode": "gdb",
      "preLaunchTask": "Build Debug",
      "setupCommands": [
        {
          "description": "Enable pretty-printing for gdb",
          "text": "-enable-pretty-printing",
          "ignoreFailures": true
        }
      ]
    }
  ]
}
```

### `.vscode/settings.json`

```json
{
  "C_Cpp.errorSquiggles": "enabled",
  "C_Cpp.intelliSenseEngine": "default",
  "C_Cpp.clang_format_fallbackStyle": "Google",
  "C_Cpp.default.cStandard": "c11",
  "C_Cpp.default.compilerPath": "/usr/bin/gcc",

  "files.autoSave": "afterDelay",
  "files.autoSaveDelay": 1000,

  "errorLens.enabled": true,
  "errorLens.fontWeight": "bold",

  "editor.rulers": [80, 120],
  "editor.wordWrap": "on",
  "editor.bracketPairColorization.enabled": true,
  "editor.formatOnSave": true,

  "files.associations": {
    "*.c": "c",
    "*.h": "c"
  }
}
```

### `.vscode/c_cpp_properties.json`

```json
{
  "configurations": [
    {
      "name": "Linux (Native)",
      "includePath": [
        "${workspaceFolder}/**",
        "/usr/include",
        "/usr/local/include"
      ],
      "defines": [],
      "compilerPath": "/usr/bin/gcc",
      "cStandard": "c11",
      "cppStandard": "c++17",
      "intelliSenseMode": "linux-gcc-x64",
      "compilerArgs": [
        "-Wall",
        "-Wextra",
        "-fsanitize=address"
      ]
    }
  ],
  "version": 4
}
```

---

## A.6 Тестування

### Тестовий файл `src/test.c`

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Testing native Linux memory leak detection\n");

    // Навмисний витік для тестування
    char *leak = malloc(100);
    leak[0] = 'A';
    printf("Allocated 100 bytes at %p\n", (void*)leak);
    // Забули free(leak); — витік!

    return 0;
}
```

### Запуск AddressSanitizer

1. Відкрийте `src/test.c`
2. `Ctrl+Shift+P` → **"Tasks: Run Task"** → **"Run ASan (Terminal)"**

**Очікуваний результат:**

```
Testing native Linux memory leak detection
Allocated 100 bytes at 0x...

=================================================================
==PID==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 100 byte(s) in 1 object(s) allocated from:
    #0 0x... in malloc
    #1 0x... in main src/test.c:7

SUMMARY: AddressSanitizer: 100 byte(s) leaked in 1 allocation(s).
```

**AddressSanitizer працює! ✅**

### Запуск Valgrind

`Ctrl+Shift+P` → **"Run Valgrind Quick"**

```
==PID== HEAP SUMMARY:
==PID==     in use at exit: 100 bytes in 1 blocks
==PID== 100 bytes in 1 blocks are definitely lost
==PID==    at 0x...: malloc
==PID==    by 0x...: main (test.c:7)
```

**Valgrind працює! ✅**

### Запуск GDB

1. Breakpoint на рядку `printf("Testing...");` (клік на номері рядка)
2. `F5`

**Програма зупинилась на breakpoint — GDB працює! ✅**

---

## A.7 Корисні alias (опціонально)

Додайте до `~/.bashrc`:

```bash
# Швидкий перехід до проектів
alias proj='cd ~/C_Projects'

# Компіляція + ASan одною командою
crun() {
    gcc -g -fsanitize=address -fsanitize=leak "$1" -o "${1%.c}_asan" \
    && ./"${1%.c}_asan"
}

# Компіляція + Valgrind одною командою
cval() {
    gcc -g "$1" -o "${1%.c}_debug" \
    && valgrind --leak-check=full ./"${1%.c}_debug"
}
```

```bash
source ~/.bashrc

# Використання:
crun src/test.c    # компіляція та запуск з ASan
cval src/test.c    # компіляція та запуск з Valgrind
```

---

## A.8 Чеклист Варіант А

- [ ] `gcc`, `valgrind`, `cppcheck`, `gdb` встановлені та перевірені
- [ ] VS Code встановлений
- [ ] Розширення `ms-vscode.cpptools` встановлене
- [ ] Проект створений у `~/C_Projects/`
- [ ] Всі 4 файли `.vscode/` створені
- [ ] ASan — тест пройшов ✅
- [ ] Valgrind — тест пройшов ✅
- [ ] GDB — тест пройшов ✅

**Час налаштування: 10-15 хвилин**

---

---

# ▶ ВАРІАНТ Б: Розробка через Docker

## Архітектура

```
┌──────────────────── Linux ─────────────────────┐
│                                                  │
│  VS Code                                         │
│    └── Dev Containers extension                  │
│           │ підключається через Unix socket       │
│           ▼                                      │
│  Docker Engine (daemon)                          │
│    └── Контейнер (debian:bookworm-slim)          │
│          ├── gcc / g++                           │
│          ├── gdb                                 │
│          ├── valgrind                            │
│          └── AddressSanitizer                   │
│                                                  │
│  ~/C_Projects/my_project/  ← файли тут          │
│  (bind mount → /workspace у контейнері)          │
└──────────────────────────────────────────────────┘
```

**Ключовий момент:** файли зберігаються в Linux FS хоста (`~/C_Projects/`), монтуються в контейнер. Ніякого Windows, ніяких прошарків — bind mount між двома Linux FS, максимальна швидкість.

---

## Б.1 Встановлення Docker Engine

### Крок 1: Видалення старих версій

```bash
sudo apt remove docker docker-engine docker.io containerd runc 2>/dev/null
```

### Крок 2: Залежності та GPG-ключ

```bash
sudo apt update
sudo apt install -y ca-certificates curl gnupg lsb-release

sudo install -m 0755 -d /etc/apt/keyrings

curl -fsSL https://download.docker.com/linux/$(. /etc/os-release && echo "$ID")/gpg \
  | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg

sudo chmod a+r /etc/apt/keyrings/docker.gpg
```

> **Примітка:** команда автоматично визначає `ubuntu` або `debian` через `$ID`.

### Крок 3: Додавання репозиторію

**Для Ubuntu:**

```bash
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] \
  https://download.docker.com/linux/ubuntu \
  $(lsb_release -cs) stable" \
  | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
```

**Для Debian:**

```bash
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] \
  https://download.docker.com/linux/debian \
  $(lsb_release -cs) stable" \
  | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
```

### Крок 4: Встановлення

```bash
sudo apt update

sudo apt install -y \
  docker-ce \
  docker-ce-cli \
  containerd.io \
  docker-buildx-plugin \
  docker-compose-plugin
```

### Крок 5: Запуск без sudo

```bash
sudo usermod -aG docker $USER
newgrp docker
```

### Крок 6: Автозапуск через systemd

```bash
sudo systemctl enable docker
sudo systemctl start docker
```

### Крок 7: Перевірка

```bash
docker run hello-world
```

**Очікуваний результат:**

```
Hello from Docker!
This message shows that your installation appears to be working correctly.
```

**Docker Engine встановлений! ✅**

---

## Б.2 Встановлення VS Code

Аналогічно Варіанту А — див. розділ **A.2**.

### Розширення для Варіанту Б

```bash
# Dev Containers (замість Remote WSL)
code --install-extension ms-vscode-remote.remote-containers

# C/C++
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cpptools-extension-pack

# Рекомендовані
code --install-extension usernamehw.errorlens
code --install-extension ms-vscode.hexeditor
```

> **Різниця від Варіанту А:** замість локальних C++ розширень — вони встановляться **всередині контейнера** через `devcontainer.json`. На хості потрібен лише `remote-containers`.

---

## Б.3 Створення проекту

```bash
mkdir -p ~/C_Projects/my_project/{.devcontainer,.vscode,src}
cd ~/C_Projects/my_project
```

### Структура проекту

```
~/C_Projects/my_project/
├── .devcontainer/
│   ├── devcontainer.json     ← конфіг Dev Containers
│   └── Dockerfile            ← образ контейнера
├── .vscode/
│   ├── tasks.json
│   ├── launch.json
│   ├── settings.json
│   └── c_cpp_properties.json
└── src/
    └── main.c
```

---

## Б.4 Конфігурація Dev Container

### `.devcontainer/Dockerfile`

```dockerfile
FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    gcc \
    g++ \
    gdb \
    valgrind \
    cppcheck \
    clang \
    clang-tools \
    libasan8 \
    ca-certificates \
    git \
    && rm -rf /var/lib/apt/lists/*

# Непривілейований користувач
ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=${USER_UID}

RUN groupadd --gid ${USER_GID} ${USERNAME} \
    && useradd --uid ${USER_UID} --gid ${USER_GID} -m ${USERNAME}

WORKDIR /workspace
USER ${USERNAME}
CMD ["/bin/bash"]
```

### `.devcontainer/devcontainer.json`

```json
{
  "name": "C Dev Container",

  "build": {
    "dockerfile": "Dockerfile",
    "context": ".."
  },

  "remoteUser": "developer",

  "workspaceFolder": "/workspace",
  "workspaceMount": "source=${localWorkspaceFolder},target=/workspace,type=bind,consistency=cached",

  // ОБОВ'ЯЗКОВО для GDB
  "runArgs": [
    "--cap-add=SYS_PTRACE",
    "--security-opt", "seccomp=unconfined"
  ],

  "customizations": {
    "vscode": {
      "extensions": [
        "ms-vscode.cpptools",
        "ms-vscode.cpptools-extension-pack",
        "usernamehw.errorlens",
        "ms-vscode.hexeditor"
      ],
      "settings": {
        "C_Cpp.default.compilerPath": "/usr/bin/gcc",
        "terminal.integrated.defaultProfile.linux": "bash"
      }
    }
  },

  "postCreateCommand": "echo '=== Versions ===' && gcc --version && valgrind --version && cppcheck --version && gdb --version | head -1"
}
```

---

## Б.5 Конфігураційні файли VS Code

`tasks.json`, `launch.json`, `settings.json` — **повністю ідентичні Варіанту А**.

Єдина відмінність — поле `"name"` у `c_cpp_properties.json`:

### `.vscode/c_cpp_properties.json`

```json
{
  "configurations": [
    {
      "name": "Docker (Linux)",
      "includePath": [
        "${workspaceFolder}/**",
        "/usr/include",
        "/usr/local/include"
      ],
      "defines": [],
      "compilerPath": "/usr/bin/gcc",
      "cStandard": "c11",
      "cppStandard": "c++17",
      "intelliSenseMode": "linux-gcc-x64",
      "compilerArgs": [
        "-Wall",
        "-Wextra",
        "-fsanitize=address"
      ]
    }
  ],
  "version": 4
}
```

> Решта трьох файлів `.vscode/` — скопіюйте з Варіанту А без змін.

---

## Б.6 Відкриття проекту в контейнері

```bash
cd ~/C_Projects/my_project
code .
```

**Варіант 1 — автоматично:**
VS Code побачить `.devcontainer/` і запропонує:
> "Folder contains a Dev Container configuration file. Reopen folder to develop in a container."

Натисніть **"Reopen in Container"**.

**Варіант 2 — вручну:**
`Ctrl+Shift+P` → **"Dev Containers: Reopen in Container"**

**Що відбудеться:**
1. Docker збере образ (~2-5 хв при першому запуску)
2. VS Code підключиться до контейнера
3. Статус-бар внизу: **"Dev Container: C Dev Container"** ✅
4. Термінал всередині контейнера — `debian:bookworm-slim`

---

## Б.7 Тестування

Тестовий файл та кроки — **ідентичні Варіанту А** (розділ A.6).

Після запуску `postCreateCommand` у терміналі VS Code побачите:

```
=== Versions ===
gcc (Debian 12.2.0) 12.2.0
valgrind-3.19.0
Cppcheck 2.10
GNU gdb 13.1
```

**Всі інструменти доступні всередині контейнера! ✅**

---

## Б.8 Управління Docker

```bash
# Переглянути запущені контейнери
docker ps

# Переглянути образи
docker images

# Звільнити місце (невикористані образи)
docker image prune -a

# Перезібрати контейнер після зміни Dockerfile
# Ctrl+Shift+P → "Dev Containers: Rebuild Container"
```

---

## Б.9 Troubleshooting

### "Cannot connect to the Docker daemon"

```bash
sudo systemctl status docker
sudo systemctl start docker
```

### "permission denied while trying to connect to Docker daemon socket"

```bash
# Перевірити групу
groups $USER   # має бути: ... docker ...

# Якщо немає — додати і перелогінитись
sudo usermod -aG docker $USER
newgrp docker
```

### GDB "ptrace: Operation not permitted"

У `.devcontainer/devcontainer.json` обов'язково:

```json
"runArgs": [
  "--cap-add=SYS_PTRACE",
  "--security-opt", "seccomp=unconfined"
]
```

Після зміни: `Ctrl+Shift+P` → **"Dev Containers: Rebuild Container"**

### Образ збирається дуже довго

Перший збір — 3-7 хвилин (завантаження базового образу ~120 MB + встановлення пакетів). Наступні запуски — миттєво (кеш Docker).

---

## Б.10 Чеклист Варіант Б

- [ ] Docker Engine встановлений (`docker --version`)
- [ ] Поточний користувач у групі `docker` (`groups $USER`)
- [ ] Docker daemon запущений (`docker run hello-world`)
- [ ] VS Code + `ms-vscode-remote.remote-containers` встановлені
- [ ] Проект у `~/C_Projects/`
- [ ] `.devcontainer/Dockerfile` та `.devcontainer/devcontainer.json` створені
- [ ] `.vscode/` файли створені
- [ ] VS Code підключений до контейнера (статус-бар)
- [ ] ASan — тест пройшов ✅
- [ ] Valgrind — тест пройшов ✅
- [ ] GDB — тест пройшов ✅

**Час налаштування: 20-30 хвилин**

---

---

# 📊 Фінальне порівняння

## Що однакове в обох варіантах

- Інструменти: `gcc`, `valgrind`, `cppcheck`, `gdb`, `AddressSanitizer`
- Файли `tasks.json`, `launch.json`, `settings.json` — **ідентичні**
- Шляхи до компілятора (`/usr/bin/gcc`) — **ідентичні**
- Workflow в VS Code (Tasks, Debug, F5) — **ідентичний**

## Що відрізняється

| | **Варіант А** | **Варіант Б** |
|---|---|---|
| Розширення VS Code | `ms-vscode.cpptools` (локально) | `ms-vscode-remote.remote-containers` (хост) + cpptools (в контейнері) |
| `c_cpp_properties.json` name | `"Linux (Native)"` | `"Docker (Linux)"` |
| Додаткові файли | — | `.devcontainer/Dockerfile` + `devcontainer.json` |
| Статус-бар VS Code | нічого | `Dev Container: C Dev Container` |
| Оновлення середовища | `sudo apt install ...` | змінити `Dockerfile` → Rebuild |

## Швидкість I/O

```
Варіант А (нативно):
  ~/C_Projects/ → gcc → ./program
  ████████████ 100%

Варіант Б (Docker, файли в ~/):
  ~/C_Projects/ → bind mount → /workspace → gcc → ./program
  ████████████ ~98%  (bind mount у межах одного Linux ядра)

Варіант Б (Docker, файли в /mnt/... або Windows):
  Windows FS → Docker → /workspace → gcc → ./program
  ████░░░░░░░░ ~40%  (НЕ РЕКОМЕНДУЄТЬСЯ)
```

---

**Для:** Linux (Debian/Ubuntu та похідні)
**Тестовано на:** Debian 11/12, Ubuntu 20.04/22.04/24.04, Linux Mint 21
**Час налаштування:** 10-15 хв (нативно) / 20-30 хв (Docker)
