### **Как создать БД?**

1. Скачать и распаковать архив: [libosmscout-importer-v2.0.1-win64.zip](https://github.com/zedxxx/libosmscout-route/releases/download/v2.0.1/libosmscout-importer-v2.0.1-win64.zip)
2. Скачать `.osm.pbf` файл интересующего региона с [geofabrik.de](https://download.geofabrik.de/)
3. Перейти в папку импортёра (из п.1) и из командной строки запустить импорт, указав полный путь к загруженному файлу:

        ImportRoute c:\belarus-latest.osm.pbf

 БД будет создана рядом с импортируемым `.osm.pbf` файлом, в папке с именем региона (для примера выше - `c:\belarus\`).
 
 ---
 
 ### **Как подключить БД к SAS.Планете?**
 
 Для использования функции оффлайн роутинга в SAS.Планете, нужно скопировать полученную БД в папку `%SASPlanet%\osmscout\`. Туда можно копировать сразу несколько БД (каждую БД в отдельную папку) и все они будут использоваться для автоматической прокладки маршрута.
 
 Важное замечание: у БД есть понятие версии формата БД. Т.е. версия утилиты, которая создаёт БД, должна быть такой же, как и версия [`libosmscout_route.dll`](https://github.com/zedxxx/libosmscout-route/releases/download/v2.0.1/libosmscout_route-v2.0.1-win32.zip), которая используется в SAS для чтения данных из БД. При несовпадении версий, SAS покажет окно с ошибкой, примерно такого содержания:
 
![Текущая версия БД: 26, ожидаемая версия: 23](Doc/error.png)

Чтобы исправить ошибку, нужно обновить dll и утилиту до актуальной, одинаковой версии.

---

### **Как быстро происходит импорт?**

При импорте активно используются ресурсы процессора, памяти и диска.

Вот, примерные данные для ПК с процессором Intel Core 2 Duo (2 ядра, 3 ГГц), 8 ГБ ОЗУ и SSD диском:

 Регион | Размер `.pbf` | Размер БД | Время создания БД
 -------| --------------------- | ------------- | ----------------------
 [Россия, Центральный ФО](https://download.geofabrik.de/russia/central-fed-district.html)| 642 МБ | 289 МБ | 12 мин 
 [Россия, Северо-Западный ФО](https://download.geofabrik.de/russia/northwestern-fed-district.html)| 450 МБ | 145 МБ | 7 мин
 [Россия, Южный ФО](https://download.geofabrik.de/russia/south-fed-district.html)| 219 МБ | 96 МБ | 4 мин
 [Украина](https://download.geofabrik.de/europe/ukraine.html)| 615 МБ | 300 МБ | 12 мин
 [Беларусь](https://download.geofabrik.de/europe/belarus.html) | 237 МБ | 105 МБ | 4 мин
 [Казахстан](https://download.geofabrik.de/asia/kazakhstan.html) | 120 МБ | 61 МБ | 2 мин

С файлом для [России](https://download.geofabrik.de/russia.html) в 2.9 ГБ, объединяющем все федеральные округа, указанный выше компьютер не справился - на последнем этапе не хватило ОЗУ.
 
---

*Источник: https://github.com/zedxxx/libosmscout-route*