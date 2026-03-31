# 🐳 Налаштування середовища розробки на C через Docker

## Виявлення витоків пам'яті: gcc, AddressSanitizer, Valgrind, GDB

Цей гайд описує єдиний підхід для **Linux, Windows і macOS**: інструментальне середовище
упаковано у Docker-контейнер на базі `debian:bookworm-slim`. Незалежно від хостової
операційної системи — VS Code підключається до контейнера через розширення **Dev Containers**
і надає однаковий інтерфейс з тими самими інструментами.

---

## 🎯 Що ви отримаєте

```
Хостова ОС (Linux / Windows / macOS)
  └── Docker (Engine або OrbStack)
        └── Контейнер debian:bookworm-slim
              ├── gcc 12 / g++ 12
              ├── gdb 13
              ├── valgrind 3.19  (memcheck, helgrind)
              ├── AddressSanitizer  (libasan8)
              └── cppcheck 2.10

VS Code (хост) ──── Dev Containers ────▶ термінал всередині контейнера
```

### ✅ Переваги підходу

| Властивість | Результат |
|---|---|
| Один `Dockerfile` для всіх | Ідентичне середовище у Linux / Windows / macOS |
| Файли в домашній директорії хоста | Максимальна швидкість I/O |
| `Dockerfile` у git | Версіонування середовища разом з кодом |
| Ізоляція | Різні проекти — різні версії інструментів |

---

## 📋 Загальні вимоги

- Увімкнена апаратна віртуалізація (VT-x / AMD-V) — перевірте в BIOS
- ~4 GB вільного місця на диску
- ~4 GB RAM (рекомендовано 8 GB)
- Visual Studio Code

---

## 💻 Крок 2: Встановлення VS Code

<details>
<summary><b>🐧 Linux</b></summary>

```bash
curl -fsSL https://packages.microsoft.com/keys/microsoft.asc \
  | gpg --dearmor \
  | sudo tee /etc/apt/keyrings/microsoft.gpg > /dev/null

echo "deb [arch=amd64 signed-by=/etc/apt/keyrings/microsoft.gpg] \
  https://packages.microsoft.com/repos/code stable main" \
  | sudo tee /etc/apt/sources.list.d/vscode.list

sudo apt update && sudo apt install -y code
```

Або через Snap: `sudo snap install code --classic`

</details>

<details>
<summary><b>🪟 Windows</b></summary>

```powershell
winget install -e --id Microsoft.VisualStudioCode
```

Або завантажте з https://code.visualstudio.com/

</details>

<details>
<summary><b>🍎 macOS</b></summary>

```bash
brew install --cask visual-studio-code
```

Або завантажте з https://code.visualstudio.com/

</details>

---

## 🖥️ Крок 2: Встановлення Docker — оберіть свою ОС

<!-- ======================================================== -->
<details>
<summary><b>🐧 Linux (Debian / Ubuntu та похідні)</b></summary>

### Встановлення Docker Engine

Docker Engine встановлюється нативно через пакетний менеджер — жодних додаткових
застосунків не потрібно.

#### Видалення старих версій

```bash
sudo apt remove docker docker-engine docker.io containerd runc 2>/dev/null
```

#### Залежності та GPG-ключ

```bash
sudo apt update
sudo apt install -y ca-certificates curl gnupg lsb-release

sudo install -m 0755 -d /etc/apt/keyrings

curl -fsSL https://download.docker.com/linux/$(. /etc/os-release && echo "$ID")/gpg \
  | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg

sudo chmod a+r /etc/apt/keyrings/docker.gpg
```

#### Додавання репозиторію

**Ubuntu:**
```bash
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] \
  https://download.docker.com/linux/ubuntu \
  $(lsb_release -cs) stable" \
  | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
```

**Debian:**
```bash
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] \
  https://download.docker.com/linux/debian \
  $(lsb_release -cs) stable" \
  | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
```

#### Встановлення

```bash
sudo apt update

sudo apt install -y \
  docker-ce \
  docker-ce-cli \
  containerd.io \
  docker-buildx-plugin \
  docker-compose-plugin
```

#### Запуск без sudo

```bash
sudo usermod -aG docker $USER
newgrp docker
```

#### Автозапуск через systemd

```bash
sudo systemctl enable docker
sudo systemctl start docker
```

#### Перевірка

```bash
docker run hello-world
```

Має з'явитись: `Hello from Docker! This message shows that your installation appears to be working correctly.` ✅

</details>

<!-- ======================================================== -->
<details>
<summary><b>🪟 Windows 10/11 — Docker Engine у WSL2 (без Docker Desktop)</b></summary>

### Крок 1: Встановлення WSL2

Відкрийте **PowerShell як Адміністратор** (`Win+X` → Terminal (Admin)):

```powershell
wsl --install
```

Перезавантажте комп'ютер. Ubuntu встановиться автоматично.

#### Перше налаштування Ubuntu

Запустіть **Ubuntu** з меню Пуск, введіть ім'я користувача та пароль:

```
Enter new UNIX username: ivan
New password: ********
```

#### Оновлення системи

```bash
sudo apt update && sudo apt upgrade -y
```

#### Перевірка версії WSL

```bash
uname -r
# Очікується: 5.15.x-microsoft-standard-WSL2
```

---

### Крок 2: Встановлення Docker Engine у WSL2

> Всі наступні команди виконуються **всередині Ubuntu WSL2**, а не в PowerShell.

```bash
sudo apt remove docker docker-engine docker.io containerd runc 2>/dev/null

sudo apt install -y ca-certificates curl gnupg lsb-release

sudo install -m 0755 -d /etc/apt/keyrings

curl -fsSL https://download.docker.com/linux/ubuntu/gpg \
  | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg

sudo chmod a+r /etc/apt/keyrings/docker.gpg

echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] \
  https://download.docker.com/linux/ubuntu \
  $(lsb_release -cs) stable" \
  | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

sudo apt update

sudo apt install -y \
  docker-ce docker-ce-cli containerd.io \
  docker-buildx-plugin docker-compose-plugin

sudo usermod -aG docker $USER
newgrp docker
```

#### Автозапуск Docker

Перевірте наявність systemd:

```bash
systemctl --version 2>/dev/null && echo "systemd є" || echo "systemd немає"
```

**Якщо systemd є** (Ubuntu 22.04+ з WSL2 ≥ 0.67):

```bash
sudo systemctl enable docker
sudo systemctl start docker
```

Переконайтесь, що в `/etc/wsl.conf` є:

```ini
[boot]
systemd=true
```

Якщо немає — додайте та перезапустіть WSL2:

```bash
echo -e "[boot]\nsystemd=true" | sudo tee /etc/wsl.conf
```

```powershell
# У PowerShell:
wsl --shutdown
wsl
```

**Якщо systemd немає:**

```bash
echo "$USER ALL=(ALL) NOPASSWD: /usr/sbin/service docker *" \
  | sudo tee /etc/sudoers.d/docker-service

echo 'sudo service docker start > /dev/null 2>&1' >> ~/.bashrc
```

---

### Крок 3: Налаштування VS Code для WSL2 + Docker

Встановіть розширення Remote WSL:

```powershell
code --install-extension ms-vscode-remote.remote-wsl
```

У `settings.json` VS Code (User settings) додайте:

```json
{
  "dev.containers.executeInWSL": true,
  "dev.containers.executeInWSLDistro": "Ubuntu"
}
```

---

### Крок 4: Перевірка

```bash
# В Ubuntu WSL2
docker run hello-world
```

**Docker у WSL2 працює! ✅**

> **Де зберігати проекти?**
> Завжди у `~/C_Projects/` (Linux FS WSL2), а не в `/mnt/c/...` (Windows диск).
> Bind mount у межах WSL2 = 97–99% від нативної швидкості.
> Bind mount через `/mnt/c/` = ~40% швидкості.

</details>

<!-- ======================================================== -->
<details>
<summary><b>🍎 macOS — OrbStack (безкоштовна заміна Docker Desktop)</b></summary>

На macOS Docker Engine не може працювати без Linux-ядра, тому потрібна легка VM як підкладка.
**OrbStack** — найпростіший варіант: безкоштовний для особистого використання, легший і
швидший за Docker Desktop.

> **Альтернативи:** Rancher Desktop (безкоштовний і для комерційного використання),
> Colima (CLI-only, для термінальних розробників).

### Встановлення OrbStack

**Спосіб 1: Homebrew (рекомендовано)**

```bash
brew install --cask orbstack
```

**Спосіб 2: Завантаження**

Завантажте з https://orbstack.dev/ та встановіть як звичайний застосунок.

### Перший запуск

1. Відкрийте **OrbStack** з Launchpad або Spotlight
2. Дочекайтеся статусу **Running** (зелена іконка в меню-барі)
3. Команда `docker` стає доступною автоматично

### Перевірка

```bash
docker run hello-world
```

**OrbStack + Docker працює! ✅**

> **Примітка:** OrbStack автоматично запускається при старті macOS і зупиняє VM коли
> Docker не використовується — батарея не витрачається даремно.

</details>

---

## 🔌 Крок 3: Встановлення розширень VS Code

Ці команди однакові для всіх ОС. Відкрийте термінал (на Windows — термінал **Ubuntu WSL2**):

```bash
# Обов'язкове — для підключення до контейнера
code --install-extension ms-vscode-remote.remote-containers

# C/C++ підтримка (встановиться всередині контейнера)
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cpptools-extension-pack

# Підсвічування помилок прямо в коді
code --install-extension usernamehw.errorlens

# Hex редактор
code --install-extension ms-vscode.hexeditor
```

**Перевірка:**

```bash
code --list-extensions | grep containers
# ms-vscode-remote.remote-containers  ✅
```

---

## 📁 Крок 4: Створення проекту

### Де зберігати файли?

| ОС | Розташування проекту |
|---|---|
| Linux | `~/C_Projects/my_project/` |
| Windows (WSL2) | `~/C_Projects/my_project/` (в Ubuntu, **не** в `/mnt/c/`) |
| macOS | `~/C_Projects/my_project/` |

```bash
mkdir -p ~/C_Projects/my_project/{.devcontainer,.vscode,src}
cd ~/C_Projects/my_project
```

### Структура проекту

```
~/C_Projects/my_project/
├── .devcontainer/
│   ├── devcontainer.json     ← конфіг Dev Containers
│   └── Dockerfile            ← образ з інструментами
├── .vscode/
│   ├── tasks.json
│   ├── launch.json
│   ├── settings.json
│   └── c_cpp_properties.json
└── src/
    └── main.c
```

---

## 🐳 Крок 5: Конфігурація Dev Container

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
  "name": "C Memory Analysis",

  "build": {
    "dockerfile": "Dockerfile",
    "context": ".."
  },

  "remoteUser": "developer",

  "workspaceFolder": "/workspace",
  "workspaceMount": "source=${localWorkspaceFolder},target=/workspace,type=bind,consistency=cached",

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

> **Чому `--cap-add=SYS_PTRACE` та `seccomp=unconfined`?**
> GDB використовує системний виклик `ptrace`, який заблокований стандартним профілем
> безпеки Docker. Ці параметри необхідні для роботи налагоджувача всередині контейнера.

---

## ⚙️ Крок 6: Конфігурація VS Code

Всі файли `.vscode/` однакові для будь-якої ОС — вони описують роботу **всередині контейнера**.

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
        "-g", "-O0",
        "-fsanitize=address", "-fsanitize=leak",
        "-fno-omit-frame-pointer",
        "-Wall", "-Wextra",
        "${file}",
        "-o", "${fileDirname}/${fileBasenameNoExtension}_asan"
      ],
      "group": { "kind": "build", "isDefault": true },
      "problemMatcher": ["$gcc"],
      "detail": "Компіляція з AddressSanitizer",
      "presentation": { "echo": true, "reveal": "always", "focus": false, "panel": "shared" }
    },
    {
      "label": "Run ASan (Terminal)",
      "type": "shell",
      "command": "${fileDirname}/${fileBasenameNoExtension}_asan",
      "dependsOn": ["Build with AddressSanitizer"],
      "problemMatcher": [],
      "presentation": { "reveal": "always", "panel": "new", "focus": true },
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
      "presentation": { "reveal": "always", "panel": "shared" },
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
      "presentation": { "reveal": "always", "panel": "dedicated", "focus": true, "clear": true },
      "detail": "Компіляція та швидкий запуск Valgrind"
    },
    {
      "label": "Static Analysis - Cppcheck",
      "type": "shell",
      "command": "cppcheck",
      "args": [
        "--enable=warning,style,performance,portability",
        "--inconclusive", "--std=c11",
        "--suppress=missingIncludeSystem",
        "${file}"
      ],
      "problemMatcher": [],
      "presentation": { "reveal": "always", "panel": "dedicated", "focus": true, "clear": true },
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
      "presentation": { "reveal": "always", "panel": "dedicated", "focus": true, "clear": true },
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
      "name": "Docker Container",
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

## 🚀 Крок 7: Відкриття проекту в контейнері

### Linux та macOS

```bash
cd ~/C_Projects/my_project
code .
```

### Windows

```bash
# В терміналі Ubuntu WSL2
cd ~/C_Projects/my_project
code .
```

### Підключення до контейнера

VS Code побачить папку `.devcontainer/` і запропонує:
> *"Folder contains a Dev Container configuration file. Reopen folder to develop in a container."*

Натисніть **"Reopen in Container"**.

Або вручну: `Ctrl+Shift+P` → **"Dev Containers: Reopen in Container"**

**Що відбудеться:**

```
1. Docker збирає образ (~3-7 хв при першому запуску, далі миттєво)
2. Запускається контейнер з параметрами з devcontainer.json
3. VS Code підключається до контейнера
4. Виконується postCreateCommand — перевірка версій інструментів
5. Статус-бар внизу: "Dev Container: C Memory Analysis" ✅
```

Термінал у VS Code тепер знаходиться **всередині контейнера**:

```bash
# Переконайтесь самі:
uname -a           # Linux, навіть на macOS чи Windows
gcc --version      # gcc (Debian 12.2.0) 12.2.0
valgrind --version # valgrind-3.19.0
```

---

## ✅ Крок 8: Тестування

### Тестовий файл `src/test.c`

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Testing Docker memory leak detection\n");

    // Навмисний витік для тестування
    char *leak = malloc(100);
    leak[0] = 'A';
    printf("Allocated 100 bytes at %p\n", (void*)leak);
    // Забули free(leak); — витік!

    return 0;
}
```

### AddressSanitizer

1. Відкрийте `src/test.c`
2. `Ctrl+Shift+P` → **"Tasks: Run Task"** → **"Run ASan (Terminal)"**

**Очікуваний результат:**

```
Testing Docker memory leak detection
Allocated 100 bytes at 0x...

=================================================================
==PID==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 100 byte(s) in 1 object(s) allocated from:
    #0 0x... in malloc
    #1 0x... in main src/test.c:8

SUMMARY: AddressSanitizer: 100 byte(s) leaked in 1 allocation(s).
```

**AddressSanitizer працює! ✅**

### Valgrind

`Ctrl+Shift+P` → **"Tasks: Run Task"** → **"Run Valgrind Quick"**

```
==PID== HEAP SUMMARY:
==PID==     in use at exit: 100 bytes in 1 blocks
==PID== 100 bytes in 1 blocks are definitely lost
==PID==    at 0x...: malloc (vg_replace_malloc.c:...)
==PID==    by 0x...: main (test.c:8)
```

**Valgrind працює! ✅**

### GDB

1. Клікніть на номері рядка `printf("Testing...")` — з'явиться червона крапка (breakpoint)
2. `F5`

**Програма зупинилась на breakpoint — GDB працює! ✅**

---

## 🔧 Troubleshooting

<details>
<summary><b>Cannot connect to the Docker daemon</b></summary>

**Linux / WSL2:**
```bash
sudo systemctl status docker
sudo systemctl start docker
```

**macOS (OrbStack):** переконайтесь, що OrbStack запущений (іконка в меню-барі).

</details>

<details>
<summary><b>permission denied while trying to connect to Docker daemon socket</b></summary>

```bash
groups $USER   # перевірити чи є "docker" у групах
sudo usermod -aG docker $USER
newgrp docker
```

</details>

<details>
<summary><b>GDB: ptrace: Operation not permitted</b></summary>

Перевірте, що в `.devcontainer/devcontainer.json` є:

```json
"runArgs": [
  "--cap-add=SYS_PTRACE",
  "--security-opt", "seccomp=unconfined"
]
```

Після зміни: `Ctrl+Shift+P` → **"Dev Containers: Rebuild Container"**

</details>

<details>
<summary><b>Windows: Dev Containers не бачить Docker</b></summary>

У VS Code `settings.json` (User):

```json
{
  "dev.containers.executeInWSL": true,
  "dev.containers.executeInWSLDistro": "Ubuntu"
}
```

Переконайтесь, що VS Code відкрито з WSL2 (статус-бар: "WSL: Ubuntu"), а не з Windows.

</details>

<details>
<summary><b>Windows: повільна робота з файлами</b></summary>

Переконайтесь, що проект в Linux FS, а не на Windows диску:

```bash
pwd
# Має бути: /home/yourname/C_Projects/...
# НЕ: /mnt/c/Users/...
```

</details>

<details>
<summary><b>Образ збирається дуже довго</b></summary>

Перший збір завантажує базовий образ (~120 MB) і встановлює пакети — 3–7 хвилин.
Наступні запуски — миттєво (Docker кешує шари образу).

Якщо змінили `Dockerfile`: `Ctrl+Shift+P` → **"Dev Containers: Rebuild Container"**

</details>

---

## 📊 Порівняння платформ

| | **Linux** | **Windows + WSL2** | **macOS + OrbStack** |
|---|---|---|---|
| Docker Engine | ✅ Нативний | ✅ У WSL2 | ✅ Через OrbStack VM |
| Без платної ліцензії | ✅ | ✅ | ✅ (особисте використання) |
| Швидкість I/O | ✅ 100% | ✅ 97–99% (файли в `~/`) | ✅ 95–99% |
| Valgrind | ✅ | ✅ | ✅ |
| AddressSanitizer | ✅ | ✅ | ✅ |
| GDB | ✅ | ✅ | ✅ |
| Час налаштування | ~20 хв | ~35 хв | ~15 хв |

---

## 📋 Фінальний чеклист

### Спільне для всіх ОС
- [ ] Docker встановлений і працює (`docker run hello-world`)
- [ ] VS Code встановлений
- [ ] Розширення `ms-vscode-remote.remote-containers` встановлене
- [ ] Проект у `~/C_Projects/` (Linux FS)
- [ ] `.devcontainer/Dockerfile` та `.devcontainer/devcontainer.json` створені
- [ ] Всі 4 файли `.vscode/` створені
- [ ] VS Code підключений до контейнера (статус-бар: "Dev Container: C Memory Analysis")
- [ ] `postCreateCommand` показав версії інструментів без помилок
- [ ] ASan — тест пройшов ✅
- [ ] Valgrind — тест пройшов ✅
- [ ] GDB — тест пройшов ✅

### Додатково для Windows
- [ ] WSL2 встановлений (`wsl --version` показує v2)
- [ ] Docker Engine встановлений в Ubuntu WSL2
- [ ] Автозапуск Docker налаштований (systemd або `.bashrc`)
- [ ] `"dev.containers.executeInWSL": true` у VS Code settings

---

**Час налаштування:** ~20 хв (Linux / macOS) / ~35 хв (Windows)
**Розмір образу:** ~380 MB (debian:bookworm-slim + інструменти)
**Тестовано на:** Debian 12, Ubuntu 22.04/24.04, Windows 11 + WSL2, macOS 14 Sonoma
