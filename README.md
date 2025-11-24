# rzlogic

## Сборка проекта

### Зависимости
- **cmake**
- **gtest / googletest**
- **pybind11**
- **build-essential** (для сборки)

Установка всех зависимостей в Ubuntu/Debian:
```bash
sudo apt-get install -y cmake build-essential googletest libgtest-dev python3-pybind11
```

## Использование Python-модуля rzresolution
<ol>
<li>Использование без ручной сборки</li>
<ol>
<li>Установите pybind11:

sudo apt-get install -y python3-pybind11</li>
<li>Перейдите в директорию rzresolution_pkg.</li>

<li>Активируйте виртуальную среду, в которую хотите установить модуль.</li>

<li>Выполните установку:
    ```bash
    pip install .
    ```
</li>

<li>После успешной установки модуль будет доступен в Python.</li>

⚠️ Если после установки модуль всё ещё не находится интерпретатором, попробуйте скопировать файл *.so из rzresolution_pkg в директорию, где расположен ваш *.py файл, выполняющий импорт.
</ol>

<li>Использование со сборкой</li>
<ol>
<li>Соберите проект с помощью CMake.</li>

<li>В директории build появится файл *.so — итоговый Python-модуль.</li>

<li>Вы можете:

* либо перенести файл *.so рядом с вашим скриптом и импортировать модуль:
    ```python
    import rzresolution
    ```

* либо установить модуль аналогично пункту выше:
    ```bash
    pip install .
    ```
</li>
</ol>
</ol>

## Справка по функции

Для получения документации вызовите:
```python
help(rzresolution.make_resolution)
```

Там приведён пример использования и полное описание параметров.