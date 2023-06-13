# idz_3
Мальченко Андрей Романович БПИ212

Сделано на 5

Вариант 2

Задача о Винни-Пухе – 1 или правильные пчелы. В одном
лесу живут n пчел и один медведь, которые используют один горшок меда, вместимостью Н глотков. Сначала горшок пустой. Пока
горшок не наполнится, медведь спит. Как только горшок заполняется, медведь просыпается и съедает весь мед, после чего снова
засыпает. Каждая пчела многократно собирает по одному глотку
меда и кладет его в горшок. Пчела, которая приносит последнюю
порцию меда, будит медведя. Создать приложение, моделирующее поведение пчел и медведя. Медведя и улей с пчелами
представить в виде отдельных клиентов. Сервер обрабатывает
поступающие от них сообщения и передает их клиентам в соответствии с установленными выше правилами

Запускается сначала сервер, потом клиенты (порядок не важен). У каждого 2 аргумента, первый это IP, второй порт.
Схема работы приложения: После успешного подключения двух клиентов, с клиента пчёл сообщение на сервер, что пчела принесла мёд (После нажатия enter, как указано на клиенте).
Далее на сервере выводится информация о том, что пчела принесла мёд. Когда горшок заполняется (Вместимость по дефолту стоит 5), сервер отправляет сообщение клиенту медведю и ждёт от него 
ответа о том, что медведь заснул. На стороне клиента медведя нужно нажать enter, когда появится информация на клиенте (будет выведено на консоль).

Для завершения работы клиентов следует прописать exit, когда будет возможность (медведь ест мёд), после завершения работы двух клиентов, сервер сам завершит работу.
