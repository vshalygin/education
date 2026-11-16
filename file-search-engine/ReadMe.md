# Поисковая система

Учебный проект на C++. Ищет слова в текстовых файлах.

Две программы:
- indexer обходит папку и подпапки, считает слова и сохраняет результат в PostgreSQL. Файлы обрабатываются в нескольких потоках.
- searcher принимает запрос в консоли и показывает файлы, где есть все слова запроса. Выше те, где эти слова встречаются чаще.

Текст файлов — UTF-8, русский и английский. Запрос до 4 слов, выдача до 10 файлов. При каждом запуске индексатора старый индекс полностью удаляется.

## Что нужно

Windows, Visual Studio с C++, CMake и Ninja (можно установить вместе с Visual Studio). Используется C++20 и сборка x64.

Нужен запущенный PostgreSQL 18. В src/CMakeLists.txt прописан путь C:/Program Files/PostgreSQL/18. Если установлен в другое место — поправить.

libpqxx лежит в папке сабмодулей и собирается вместе с проектом.

Ещё нужен Boost: Locale и заголовки PropertyTree. Скачать можно с https://www.boost.org/releases/latest/ .

## Boost

Открыть x64 Native Tools Command Prompt для своей Visual Studio. Перейти в распакованную папку Boost и выполнить:

```bat
set "BOOST_ROOT=C:\Libraries\boost"
setx BOOST_ROOT "%BOOST_ROOT%"
bootstrap.bat
b2 --with-locale address-model=64 variant=debug,release link=static runtime-link=shared boost.locale.icu=off install --prefix="%BOOST_ROOT%"
```

Путь можно выбрать другой. BOOST_ROOT должен указывать на папку установки. Если Boost уже собран с такими параметрами, повторять сборку не надо.

После установки переменной перезапустить Visual Studio. Boost подключается статически, runtime MSVC — динамический.

## Сборка

В x64 Native Tools Command Prompt перейти в корень проекта:

```bat
cmake --preset x64-debug
cmake --build out/build/x64-debug
```

Либо открыть папку проекта в Visual Studio, выбрать x64-debug и собрать.

## Запуск

В config.ini указать подключение к своей БД, папку документов и расширения файлов. База должна уже существовать, таблицы индексатор создаст сам.

Для проверки можно указать полный путь к папке example. Там есть готовые файлы и вложенные каталоги.

Команды из корня проекта:

```bat
chcp 65001
out\build\x64-debug\src\indexer\indexer.exe config.ini
out\build\x64-debug\src\searcher\searcher.exe config.ini
```

Если Windows не находит libpq.dll, добавить папку bin PostgreSQL в PATH. Например, для текущей консоли:

```bat
set "PATH=C:\Program Files\PostgreSQL\18\bin;%PATH%"
```

Сначала индексатор, потом поисковик. В поисковике ввести слова и нажать Enter. Пустая строка — выход.

На примерах можно искать «привет мир», «кот дом», «search engine». Для «привет мир» ожидаются два файла с оценками 14 и 11.
