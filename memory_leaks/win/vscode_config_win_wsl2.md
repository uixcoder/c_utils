# Налаштування VS Code + WSL2 для відслідковуання витоків пам'яті у програмах на мові С 

## 🎯 Що таке WSL2?

**Windows Subsystem for Linux 2** - це повноцінний Linux kernel, що працює в Windows. Ви отримуєте справжній Ubuntu в Windows!

### ✅ Що працює в WSL2:

- ✅ **GCC** - компілятор
- ✅ **AddressSanitizer** - повна підтримка
- ✅ **Valgrind** - повна підтримка
- ✅ **Cppcheck** - статичний аналізатор
- ✅ **Clang** - альтернативний компілятор
- ✅ **GDB** - дебагер
- ✅ **Всі Linux інструменти**

**По суті, ВСЕ працює як на нативному Linux! Весь гайд працює на 100%!**

---

## 📋 Вимоги

- Windows 10 (version 2004 або новіше) або Windows 11
- Увімкнена віртуалізація в BIOS (VT-x для Intel або AMD-V для AMD)
- ~4 GB вільного місця на диску

---

## 🚀 Встановлення WSL2

### Крок 1: Встановлення WSL2 (5 хвилин)

#### Для Windows 11 або Windows 10 (version 2004+):

1. **Відкрийте PowerShell як Адміністратор**
   - Натисніть `Win+X`
   - Виберіть "Windows PowerShell (Admin)" або "Terminal (Admin)"

2. **Виконайте одну команду:**

```powershell
wsl --install
```

3. **Перезавантажте комп'ютер**

4. **Ubuntu встановиться автоматично** при першому запуску

**Це все! WSL2 встановлений!** ✅

---

#### Для старіших версій Windows 10:

<details>
<summary>Клікніть щоб розгорнути інструкції для старих версій</summary>

**1. Увімкніть WSL:**

```powershell
dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
```

**2. Увімкніть Virtual Machine Platform:**

```powershell
dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
```

**3. Перезавантажте комп'ютер**

**4. Завантажте WSL2 kernel update:**

Завантажте з: https://aka.ms/wsl2kernel та встановіть

**5. Встановіть WSL2 за замовчуванням:**

```powershell
wsl --set-default-version 2
```

**6. Встановіть Ubuntu з Microsoft Store:**

- Відкрийте Microsoft Store
- Пошук: "Ubuntu"
- Встановіть "Ubuntu 24.04 LTS" або "Ubuntu 22.04 LTS"

</details>

---

### Крок 2: Перше налаштування Ubuntu (2 хвилини)

1. **Запустіть Ubuntu** з меню Пуск

2. **Почекайте** поки завершиться встановлення (1-2 хвилини)

3. **Створіть користувача:**
   - Введіть ім'я користувача (тільки малі літери, без пробілів)
   - Введіть пароль (не буде відображатися при введенні - це нормально)
   - Підтвердіть пароль

**Приклад:**
```
Enter new UNIX username: ivan
New password: ********
Retype new password: ********
```

4. **Готово!** Ви в Ubuntu терміналі ✅

---

### Крок 3: Оновлення системи (2 хвилини)

```bash
# Оновіть списки пакетів
sudo apt update

# Оновіть встановлені пакети
sudo apt upgrade -y
```

**Примітка:** При першому `sudo` введіть пароль який створили.

---

### Крок 4: Встановлення інструментів розробки (3 хвилини)

**Одна команда для встановлення всього:**

```bash
sudo apt install -y \
  build-essential \
  gcc \
  g++ \
  gdb \
  libasan6 \
  libasan8 \
  valgrind \
  cppcheck \
  clang \
  clang-tools
```

**Перевірка встановлення:**

```bash
gcc --version
valgrind --version
cppcheck --version
```

**Очікуваний вивід:**
```
gcc (Ubuntu 13.2.0-23ubuntu4) 13.2.0
valgrind-3.22.0
Cppcheck 2.13.0
```

**Якщо всі три команди працюють - інструменти встановлені! ✅**

---

## 💻 Встановлення VS Code

### Крок 1: Встановлення VS Code на Windows

**Спосіб 1: Завантаження**

Завантажте з: https://code.visualstudio.com/

**Спосіб 2: Через winget (Windows 11)**

```powershell
winget install -e --id Microsoft.VisualStudioCode
```

---

### Крок 2: Встановлення розширень

**ОБОВ'ЯЗКОВІ розширення:**

```bash
# Розширення для роботи з WSL
code --install-extension ms-vscode-remote.remote-wsl

# C/C++ розширення
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cpptools-extension-pack

# Рекомендовані
code --install-extension usernamehw.errorlens
code --install-extension ms-vscode.hexeditor
```

**Перевірка:**
```bash
code --list-extensions
```

Має бути принаймні:
- `ms-vscode-remote.remote-wsl` ✅
- `ms-vscode.cpptools` ✅

---

## 📁 Створення проекту

### Крок 1: Створіть папку проекту в WSL

**ВАЖЛИВО:** Створюйте проекти в WSL файловій системі, а НЕ на Windows диску!

```bash
# Створіть папку для проектів
mkdir -p ~/C_Projects
cd ~/C_Projects

# Створіть перший проект
mkdir my_first_project
cd my_first_project

# Створіть папку для конфігурацій
mkdir .vscode
```

**Чому саме ~/C_Projects?**
```
✅ ПРАВИЛЬНО (швидко):
~/C_Projects/              # WSL файлова система
/home/username/C_Projects/

❌ НЕПРАВИЛЬНО (повільно):
/mnt/c/Users/...          # Windows диск
```

---

### Крок 2: Відкрийте проект у VS Code

```bash
# З Ubuntu терміналу WSL
cd ~/C_Projects/my_first_project
code .
```

**VS Code автоматично підключиться до WSL!**

Ви побачите внизу ліворуч: **"WSL: Ubuntu"** ✅

---

## ⚙️ Конфігураційні файли

Створіть ці файли в папці `.vscode/` вашого проекту:

### `.vscode/tasks.json`

<details>
<summary>Клікніть щоб розгорнути повний файл</summary>

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
      "problemMatcher": [
        "$gcc"
      ],
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
      "dependsOn": [
        "Build with AddressSanitizer"
      ],
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
        "-g",
        "-O0",
        "-Wall",
        "-Wextra",
        "${file}",
        "-o",
        "${fileDirname}/${fileBasenameNoExtension}_debug"
      ],
      "group": "build",
      "problemMatcher": [
        "$gcc"
      ],
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
      "problemMatcher": [
        "$gcc"
      ],
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
      "presentation": {
        "reveal": "silent"
      },
      "detail": "Видалити скомпільовані файли"
    }
  ]
}
```
</details>

---

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

---

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
  
  "terminal.integrated.defaultProfile.windows": "Ubuntu (WSL)",
  
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

---

### `.vscode/c_cpp_properties.json`

```json
{
  "configurations": [
    {
      "name": "WSL",
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

## ✅ Тестування

### Крок 1: Створіть тестовий файл

Створіть файл `test.c`:

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Testing WSL2 memory leak detection\n");
    
    // Навмисний витік для тестування
    char *leak = malloc(100);
    leak[0] = 'A';
    
    printf("Allocated 100 bytes at %p\n", (void*)leak);
    // Забули free(leak); - витік!
    
    return 0;
}
```

---

### Крок 2: Запустіть AddressSanitizer

1. **Відкрийте** `test.c` в VS Code
2. Натисніть `Ctrl+Shift+P`
3. Виберіть "Tasks: Run Task"
4. Виберіть **"Run ASan (Terminal)"**

**Очікуваний результат:**

```
Testing WSL2 memory leak detection
Allocated 100 bytes at 0x...

=================================================================
==12345==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 100 byte(s) in 1 object(s) allocated from:
    #0 0x... in malloc
    #1 0x... in main test.c:7

SUMMARY: AddressSanitizer: 100 byte(s) leaked in 1 allocation(s).
```

**Якщо бачите це - AddressSanitizer працює! ✅**

---

### Крок 3: Запустіть Valgrind

1. `Ctrl+Shift+P`
2. "Tasks: Run Task"
3. **"Run Valgrind Memcheck"**
4. Відкрийте файл `valgrind-out.txt`

**Очікується:**

```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 100 bytes in 1 blocks
==12345==   total heap usage: 2 allocs, 1 frees, 1,124 bytes allocated
==12345==
==12345== 100 bytes in 1 blocks are definitely lost in loss record 1 of 1
==12345==    at 0x4844818: malloc
==12345==    by 0x109XXX: main (test.c:7)
```

**Якщо бачите "definitely lost" - Valgrind працює! ✅**

---

### Крок 4: Тест дебагера

1. Поставте breakpoint на рядку `printf("Testing...");`
   - Клік на номері рядка ліворуч
2. Натисніть `F5`
3. Програма зупиниться на breakpoint

**Якщо зупинився - GDB працює! ✅**

---

## 📁 Робота з файлами WSL

### Доступ до WSL файлів з Windows

**Спосіб 1: Windows Explorer**

1. Натисніть `Win+R`
2. Введіть: `\\wsl$\Ubuntu`
3. Ваші файли в `/home/username/C_Projects/`

**Спосіб 2: З WSL терміналу**

```bash
# Відкрити поточну папку в Windows Explorer
explorer.exe .
```

---

### Доступ до Windows файлів з WSL

Windows диски доступні в WSL через `/mnt/`:

```bash
# C: диск
cd /mnt/c/Users/YourName/Documents

# D: диск
cd /mnt/d/
```

**⚠️ Примітка:** Працювати з файлами на `/mnt/c/` повільніше, ніж в `~/`

**Рекомендація:** Зберігайте проекти в `~/C_Projects/`

---

## 🔧 Troubleshooting

### Проблема: WSL не встановлюється

**Перевірте версію Windows:**

```powershell
winver
```

Потрібно: **Windows 10 version 2004** або новіше

**Рішення:** Оновіть Windows через Settings → Update & Security

---

### Проблема: Віртуалізація не увімкнена

**Симптоми:** Помилка при встановленні WSL

**Рішення:**

1. Перезавантажте комп'ютер в BIOS (F2, F10, Del при старті)
2. Знайдіть опцію **Virtualization Technology** або **VT-x** (Intel) / **AMD-V** (AMD)
3. Увімкніть (Enabled)
4. Збережіть і вийдіть (F10)

---

### Проблема: "WSL 2 requires an update to its kernel component"

**Рішення:**

Завантажте kernel update: https://aka.ms/wsl2kernel

---

### Проблема: VS Code не бачить WSL

**Рішення:**

1. Перевірте чи встановлене WSL розширення:
   ```bash
   code --install-extension ms-vscode-remote.remote-wsl
   ```

2. Перезапустіть VS Code

3. Перевірте WSL:
   ```powershell
   wsl --list --verbose
   ```

   Має показати Ubuntu з VERSION 2

---

### Проблема: Повільна робота

**Причина:** Файли на Windows диску (`/mnt/c/`)

**Рішення:**

Перемістіть проект в WSL файлову систему:

```bash
# З /mnt/c/... (повільно)
# В ~/C_Projects/ (швидко)

cp -r /mnt/c/Users/YourName/project ~/C_Projects/
cd ~/C_Projects/project
```

---

### Проблема: "Permission denied" при компіляції

**Рішення:**

```bash
chmod +x ./program_asan
```

---

## 💡 Поради для WSL2

### 1. Використовуйте Windows Terminal

**Встановлення:**
- Microsoft Store → "Windows Terminal"

**АБО:**
```powershell
winget install Microsoft.WindowsTerminal
```

**Переваги:**
- Краща підтримка кольорів
- Вкладки
- Розділені панелі
- Швидше

---

### 2. Alias для швидкого доступу

Додайте в `~/.bashrc`:

```bash
# Швидкий перехід до проектів
alias proj='cd ~/C_Projects'

# Швидка компіляція і запуск з ASan
crun() {
    gcc -g -fsanitize=address "$1" -o "${1%.c}_asan" && ./"${1%.c}_asan"
}

# Швидкий Valgrind
cval() {
    gcc -g "$1" -o "${1%.c}_debug" && \
    valgrind --leak-check=full ./"${1%.c}_debug"
}
```

**Використання:**
```bash
proj              # Перейти до ~/C_Projects
crun test.c       # Компіляція + ASan
cval test.c       # Компіляція + Valgrind
```

---

### 3. Оновлення системи

Регулярно оновлюйте Ubuntu в WSL:

```bash
sudo apt update && sudo apt upgrade -y
```

---

## 🎯 Підсумок WSL2

### ✅ Що отримали:

- Повноцінний Linux в Windows
- Всі інструменти працюють (gcc, asan, valgrind, cppcheck, gdb)
- VS Code інтеграція
- 100% сумісність з Linux гайдами
- Швидка продуктивність

### 📋 Швидкий чеклист:

- [ ] WSL2 встановлений
- [ ] Ubuntu налаштований
- [ ] Інструменти встановлені (gcc, valgrind, cppcheck)
- [ ] VS Code + WSL розширення
- [ ] Проект створений в ~/C_Projects/
- [ ] Конфігураційні файли створені
- [ ] Тести пройшли успішно

**Якщо всі пункти виконані - готово до розробки! 🚀**

---

## 📚 Наступні кроки

Тепер використовуйте **основний гайд** без жодних змін:
- Всі команди працюють
- Всі інструменти доступні
- Весь workflow ідентичний Linux

**З WSL2 ви маєте повноцінне Linux середовище в Windows!**

---

**Для:** Windows користувачів  
**Тестовано на:** Windows 10 (2004+), Windows 11  
**Час налаштування:** 15-20 хвилин
