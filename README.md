# Отчёт по ИДЗ №4 АВС, вариант 2 - Хорасанджян Л. А., БПИ218

## Комментарии к решению
Хотелось бы внести ясность в структуру репозитория: в папке grade_4 приведены 2 подпапки, содержащие старый и новый подход к решению.
В дальнейшем для всех критериев, включая критерии на оценку 4, я буду опираться на **новое решение**, которое в папке **"new_version"**,
соответственно. Папку "old_version" я решил всё-таки оставить лишь в архивных целях, её прошу не рассматривать всерьёз, она скорее служит
лишь в целях показать, какая мысль была, и к какой мысли я в итоге пришёл. Конечно, мог бы это показать и как два разных коммита, но
но мне кажется, что так выглядит более отчётливо 2 разных подхода, а не "одно — усовершенствование другого". Ещё уточню, что иногда я пишу
"глоток мёда", а иногда "капля мёда" — в моих рассуждениях это одно и то же.

## Выполненные критерии на 4 балла

### Условие задачи
Задача о Винни-Пухе - 1 или правильные пчелы.  
В одном лесу живут n пчел и один медведь, которые используют один горшок меда, вместимостью Н глотков. Сначала горшок пустой.
Пока горшок не наполнится, медведь спит. Как только горшок заполняется, медведь просыпается и съедает весь мед, после чего снова засыпает.
Каждая пчела многократно собирает по одному глотку меда и кладет его в горшок. Пчела, которая приносит последнюю порцию меда, будит медведя.
Создать многопоточное приложение, моделирующее поведение пчел и медведя.

### Модель параллельных вычислений
При разработке данной многопоточной программы я опирался на идею итеративного параллелизма, которая реализуется в функции fillHoneyPot.
Более подробное пояснение работы программы в виде рассмотрения предметных объектов описывается в колонке на оценку 5.

### Входные и выходные данные
Допустимые диапазоны входных данных:
1. кол-во пчёл (потоков): от 1 включительно до 10 включительно (т. е. \[1; 10\]);
2. объём общего горшка мёда: от 10 включительно до ста миллиардов включительно (т. е. \[10; 100.000.000.000\]).
  
Возможные выходные данные:  
При одних и тех же входных данных вариативность вывода на экран будет заключаться лишь в том, какая пчела по номеру разбудила медведя.

### Сихнропримитив
Для решения задачи в качестве сихнропримитива я использовал мьютекс:
```cpp
    pthread_mutex_lock(&mutex);
    honey_pot += local_pot;
    if (honey_pot != total_capacity && honey_pot == local_capacity * count_of_threads) {
        for (long long i = 0; i < total_capacity - local_capacity * count_of_threads; ++i) {
            ++honey_pot;
        }
    }
    pthread_mutex_unlock(&mutex);
```

### Консольный ввод данных
Здесь ожидается консольный ввод данных от пользователя:
```cpp
    do {
        std::cout << "select count of bees in range [1;10]: ";
        std::cin >> count_of_threads;
    } while (count_of_threads < 1 || count_of_threads > 10);

    std::cout << std::endl;

    do {
        std::cout << "select size of honey pot in range [10; 100.000.000.000]: ";
        std::cin >> total_capacity;
    } while (total_capacity < 10 || total_capacity > 100000000000);
```

## Выполненные критерии на 5 баллов

### Описание сценария работы
Пчёлы, как объекты, представляющие из себя потоки, совместно выполняют одно и то же действие — налить один глоток мёда в горшок. Моя реализация заключается в том, что пчёлы
изначально заполняют каждый свой собственный горшок, чья вместимость равна целочисленному делению вместимости общего большого горшка (из
которой уже будет пить медведь) на общее количество пчёл. После того, как i-ая пчела заполнила свой горшочек полностью, она подлетает к общему горшку,
запрещая другим пчёлам как либо на него в данный момент воздействовать (по сути говорю тут о применении мьютекса), спокойно переливает в него всё
содержимое своего горшочка. Каждая пчела после такого одиночного заполнения общего горшка проверяет два условия: 1) незаполненность общего
горшка до краёв (т. е. полностью); 2) текущее кол-во глотков мёда в общем горшке равно ли размеру личного горшочка пчёлки, умноженному на
количество пчёлок. Если оба условия выполняются (а это по логике **может** произойти только с последней пчёлкой (НЕ выполнится с последней
только если вместимость общего горшка поделилась без остатка на количество пчёлок)), то последняя, самая медленная пчёлка доливает в одиночку
оставшиеся глотки, чтобы заполнить общий горшок до краёв. При таком подходе решения есть гарантия, что последней пчёлке в одиночку придётся доливать
не более чем ('текущее_количество_пчёлок' - 1) глотков мёда. В моём решении максимальное кол-во потоков (т. е. пчёлок) равно 10, а значит
последний поток при любых входных данных произведёт не более 9 инкрементаций заполненности общего горшка.  
  
После такого подробного и, возможно, сложного объяснения, для простоты понимания я приведу 3 наглядных примера:
1. Имеем общий горшок вместимостью 10 глотков и 6 пчёл, в таком случае у каждой пчелы имеется свой собственный горшок вместимостью 1 глоток
(то есть 10/6 = 1). Все пчёлы, кроме последней, заливают в свой личный горшочек по 1 глотку мёда и переливают в одиночку в общий горшочек.
Таким образом, от 5 пчёл в горшочке будет 5 глотков, а эти 5 пчёл пойдут отдыхать. 6-ая, самая медленная пчёлка наливает 6-ую каплю мёда, а затем
в качестве штрафа наливает недостающие для заполненности 4 капли мёда сразу в общий горшок. После этого общий горшок полон, а 6-ая пчёлка идёт
будить медведя кушать мёд.
2. Имеем общий горшок вместимостью 100 глотков и 3 пчелы, в таком случае у каждой пчелы имеется свой собственный горшок вместимостью 33 глотка
(то есть 100/3 = 33). Все пчёлы, кроме последней, заливают в свой личный горшочек по 33 глотков мёда и переливают в одиночку в общий горшочек.
Таким образом, от 2 пчёл в горшочке будет 2\*33=66 глотков, а эти 2 пчёл пойдут отдыхать. 3-ая, самая медленная пчёлка наливает 33 капель мёда, а затем
в качестве штрафа наливает недостающую для заполненности 100-66-33=1 каплю мёда сразу в общий горшок. После этого общий горшок полон, а 3-ая пчёлка идёт
будить медведя кушать мёд.
3. Имеем общий горшок вместимостью 100 глотков и 4 пчелы, в таком случае у каждой пчелы имеется свой собственный горшок вместимостью 25 глотков
(то есть 100/4 = 25). Все пчёлы, кроме последней, заливают в свой личный горшочек по 25 глотков мёда и переливают в одиночку в общий горшочек.
Таким образом, от 3 пчёл в горшочке будет 3\*25=75 глотков, а эти 3 пчелы пойдут отдыхать. 4-ая, самая медленная пчёлка наливает 25 капель мёда, а затем
в качестве штрафа наливает недостающую для заполненности 100-75-25=0 капель мёда сразу в общий горшок (т. е. фактически избегает штрафа).
Получается, что общий горшок уже и так полон, поэтому 4-ая пчёлка просто идёт будить медведя кушать мёд.