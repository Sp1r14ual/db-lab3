#include <sqlca.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
exec SQL begin declare section;
char db_name[50];
char user_name[10];
char user_password[10];
unsigned int count=0;
exec SQL end declare section;
void menu() {
printf("\n Введите номер запроса(1-5) или 0 для выхода \n");
}
void exitProgram() {
exec SQL DISCONNECT CURRENT;
printf("Отключено от базы данных\n");
exit(0);
}
void tablePrint() {
EXEC SQL BEGIN DECLARE SECTION;
char n_det[6];
char name[40];
char cvet[40];
int ves;
char town[40];
EXEC SQL END DECLARE SECTION;
exec SQL declare curs4 CURSOR for
Select p.*
from p;
if (sqlca.sqlcode < 0) {
printf("Ошибка объявления курсора. Код: %d %s \n Текст ошибки: %s\n ",
sqlca.sqlcode,
sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
exec SQL rollback;
return;
}
printf("\nОткрытие курсора...\n");
exec SQL begin;
exec SQL OPEN curs4;
if (sqlca.sqlcode < 0) {
printf("Ошибка открытия курсора. Код: %d %s \n Текст ошибки: %s\n ",
sqlca.sqlcode,
sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
exec SQL close curs4;
exec SQL rollback;
return;
}
int row_count = 0;
while (1) {
exec SQL FETCH curs4 INTO :n_det, :name, :cvet, :ves, :town;
if (sqlca.sqlcode < 0) {
printf("Ошибка получения строки! Код: %d (%s)\n Текст ошибки: %s\n",
sqlca.sqlcode,
sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
exec SQL close curs4;
exec SQL rollback;
return;
}
if (sqlca.sqlcode == 100) break;
if (row_count == 0) printf("n_det\tname\t\t\tcvet\t\t\tves\ttown\n");
printf("%s\t%s\t%s\t%d\t%s\n", n_det, name, cvet,ves,town);
row_count++;
}
printf("Закрытие курсора...\n\n");
exec SQL close curs4;
exec SQL commit;
if(row_count==0) {
printf("Нет данных.\n");
}
printf("Результат: %d строк(-а/и).\n", row_count);
return;
}
void task1() {
printf("Выдать число цветов деталей, поставлявшихся поставщиками, выполнявшими поставки для изделий из Парижа.\n\n");
exec SQL begin;
exec SQL
SELECT COUNT(DISTINCT p.cvet)
FROM p
JOIN spj ON spj.n_det = p.n_det 
WHERE n_post IN 
(
SELECT n_post
FROM spj
JOIN j ON j.n_izd=spj.n_izd
WHERE j.town='Париж'
)
if (sqlca.sqlcode < 0) {
printf("Ошибка запроса select! Код: %d %s \n Текст ошибки: %s\n ", sqlca.sqlcode, sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
exec SQL rollback;
return;
}
printf("Result: %d \n", count);
exec SQL commit;
return;
}
void task2() {
printf("Поменять местами названия деталей, стоящих первой и последней в списке, упорядоченном по весу и 
названию.\n\n");
tablePrint();
exec SQL begin;
printf("Выполение запроса... \n");
exec SQL
UPDATE p
SET 
name = (
CASE
WHEN name = (
SELECT name
FROM p
ORDER BY ves, name
LIMIT 1
)
THEN (
SELECT name
FROM p
ORDER BY ves DESC, name DESC
LIMIT 1
)
ELSE (
SELECT name
FROM p
ORDER BY ves, name
LIMIT 1
)
END
)
WHERE 
name = (
SELECT name
FROM p
ORDER BY ves, name
LIMIT 1
)
OR
name = (
SELECT name
FROM p
ORDER BY ves DESC, name DESC
LIMIT 1
);
if (sqlca.sqlcode < 0) {
printf("Ошибка запроса update! Код: %d (%s)\n Текст ошибки%s\n",
sqlca.sqlcode,
sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
exec SQL rollback;
return;
}
printf("Обновлено(-а) %d строк(-и). \n", sqlca.sqlerrd[2]);
exec SQL commit;
printf("\nТаблица после выполнения запроса:\n");
tablePrint();
return;
}
void task3() {
printf("Найти изделия, имеющие поставки, объем которых более чем в 2 раза превышает средний объем поставки для 
изделия."\
" Вывести номер изделия, объем поставки, средний объем поставки для изделия.\n\n");
printf("Объявление курсора...\n");
EXEC SQL BEGIN DECLARE SECTION;
char n_izd[6];
int kol;
float avg;
EXEC SQL END DECLARE SECTION;
exec SQL declare curs1 CURSOR for
SELECT spj.n_izd, spj.kol, temp.avg
FROM spj
JOIN 
(
SELECT 
j.n_izd,
ROUND(AVG(spj.kol), 2) AS avg
FROM 
j
JOIN 
spj ON j.n_izd = spj.n_izd
GROUP BY 
j.n_izd
) temp
ON spj.n_izd = temp.n_izd
WHERE spj.kol / temp.avg > 2;
if (sqlca.sqlcode < 0) {
printf("Ошибка обявления курсора! Код: %d (%s)\n Текст ошибки: %s\n",
sqlca.sqlcode,
sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
return;
}
printf("Открытие курсора...\n");
exec SQL begin;
exec SQL OPEN curs1;
if (sqlca.sqlcode < 0) {
printf("Ошибка открытия курсора! Код: %d (%s)\n Текст ошибки: %s\n",
sqlca.sqlcode,
sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
exec SQL close curs1;
exec SQL rollback;
return;
}
printf("Получение результата...\n");
int row_count = 0;
while (1) {
exec SQL FETCH curs1 INTO :n_izd, :kol, :avg;
if (sqlca.sqlcode < 0) {
printf("Ошибка получения строки! Код: %d (%s)\n Текст ошибки: %s\n",
sqlca.sqlcode,
sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
exec SQL close curs1;
exec SQL rollback;
return;
}
if (sqlca.sqlcode == 100) break;
if (row_count == 0) printf("n_izd\tkol\tavg\n");
printf("%s\t%d\t%f\n", n_izd, kol, avg);
row_count++;
}
printf("Закрытие курсора...\n");
exec SQL close curs1;
exec SQL commit;
if (row_count == 0) printf("Данные не найдены.");
else printf("Результат: %d строк(-а/и).\n", row_count);
return;
}
void task4() {
printf("Выбрать изделия, для которых не поставлялась ни одна деталь, имеющая наибольший вес.\n\n");
printf("Объявление курсора...\n");
EXEC SQL BEGIN DECLARE SECTION;
char n_izd[6];
EXEC SQL END DECLARE SECTION;
exec SQL declare curs2 CURSOR for
SELECT DISTINCT spj.n_izd
FROM spj
EXCEPT
SELECT DISTINCT n_izd
FROM j a
WHERE NOT EXISTS(
SELECT * 
FROM spj 
WHERE spj.n_izd=a.n_izd
)
UNION
SELECT spj.n_izd
FROM spj
WHERE spj.n_det in (
SELECT n_det
FROM p
WHERE ves = (SELECT max(ves) FROM p)
if (sqlca.sqlcode < 0) {
printf("Ошибка объявления курсора! Код: %d (%s)\n Текст ошибки: %s\n",
sqlca.sqlcode,
sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
exec SQL rollback;
return;
}
printf("Открытие курсора...\n");
exec SQL begin;
exec SQL OPEN curs2;
if (sqlca.sqlcode < 0) {
printf("Ошибка открытия курсора! Код: %d(%s)\n Текст ошибки: %s\n",
sqlca.sqlcode,
sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
exec SQL close curs2;
exec SQL rollback;
return;
}
printf("Получение результата...\n");
int row_count = 0;
while (1) {
exec SQL FETCH curs2 INTO :n_izd;
if (sqlca.sqlcode < 0) {
printf("Ошибка получения строки! Код: %d (%s)\n Текст ошибки: %s\n",
sqlca.sqlcode,
sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
exec SQL close curs2;
exec SQL rollback;
return;
}
if (sqlca.sqlcode == 100) break;
if (row_count == 0) printf("n_izd\n");
printf("%s\n", n_izd);
row_count++;
}
printf("Закрытие курсора...\n");
exec SQL close curs2;
exec SQL commit;
if (row_count == 0) printf("Данные не найдены.");
else printf("Результат: %d строк(-а/и).\n", row_count);
return;
}
void task5() {
printf("Выдать полную информацию об изделиях, для которых поставлялись ТОЛЬКО детали из последнего по алфавиту 
города.\n");
printf("Объявление курсора...\n");
EXEC SQL BEGIN DECLARE SECTION;
char n_izd[6];
char name[40];
char town[40];
int name_;
int town_;
EXEC SQL END DECLARE SECTION;
exec SQL
declare curs3 CURSOR for
SELECT DISTINCT j.n_izd, j.name, j.town
FROM spj
JOIN j ON j.n_izd = spj.n_izd
WHERE n_det IN (
SELECT n_det
FROM p
WHERE town = (SELECT MAX(town) FROM p)
)
EXCEPT
SELECT DISTINCT j.n_izd, j.name, j.town
FROM spj
JOIN j ON j.n_izd = spj.n_izd
WHERE n_det NOT IN (
SELECT n_det
FROM p
WHERE town = (SELECT MAX(town) FROM p)
);
if (sqlca.sqlcode < 0) {
printf("Ошибка объявления курсора! Код: %d (%s)\n Текст ошибки: %s\n",
sqlca.sqlcode,
sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
exec SQL rollback;
return;
}
printf("Открытие курсора...\n");
exec SQL begin;
exec SQL OPEN curs3;
if (sqlca.sqlcode < 0) {
printf("Ошибка открытия курсора! Код: %d(%s)\n Текст ошибки: %s\n",
sqlca.sqlcode,
sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
exec SQL close curs3;
exec SQL rollback;
return;
}
printf("Получение результата...\n");
int row_count = 0;
while (1) {
exec SQL FETCH curs3 INTO :n_izd, :name indicator name_, :town indicator town_;
if (sqlca.sqlcode < 0) {
printf("Ошибка получения строки! Код: %d (%s)\n Текст ошибки: %s\n",
sqlca.sqlcode,
sqlca.sqlstate,
sqlca.sqlerrm.sqlerrmc);
exec SQL close curs3;
exec SQL rollback;
return;
}
if (sqlca.sqlcode == 100) break;
if (row_count == 0) printf("n_izd\tname\t\t\ttown\n");
if(name_<0) strcpy(name,"Нет данных");
if(town_<0) strcpy(town,"Нет данных");
printf("%s\t%s\t%s\n", n_izd, name, town);
row_count++;
}
printf("Закрытие курсора...\n");
exec SQL close curs3;
exec SQL commit;
if (row_count == 0) printf("Данные не найдены.");
else printf("Результат: %d строк(-а/и).\n", row_count);
return;
}
int main() {
strcpy(db_name, "students");
strcpy(user_name, "pmi-b1611");
strcpy(user_password, "SeregaBankir1131!");
printf("Подключение к базе данных...\n");
exec SQL CONNECT TO :db_name USER :user_name using :user_password;
if (sqlca.sqlcode < 0) {
printf("Ошибка аутентификации. \n");
exit(1);
} else {
printf("Подключено к базе данных \"students\" под пользователем %s \n", user_name);
}
exec SQL SET search_path TO pmib1611, public;
int cmd = 1;
menu();
while (cmd != 0) {
printf("Введите команду: ");
scanf("%d", &cmd);
switch (cmd) {
case 1:
task1();
printf("\n 6 - меню \n");
break;
case 2:
task2();
printf("\n 6 - меню \n");
break;
case 3:
task3();
printf("\n 6 - меню \n");
break;
case 4:
task4();
printf("\n 6 - меню \n");
break;
case 5:
task5();
printf("\n 6 - меню\n");
break;
case 6:
menu();
break;
case 0:
exitProgram();
break;
default:
menu();
}
}
return 0;
}
