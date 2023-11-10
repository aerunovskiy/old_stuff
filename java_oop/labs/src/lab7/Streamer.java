/*
 * С использованием streamAPI реализовать следующие методы:
 *   1. метод, возвращающий среднее значение списка целых чисел
 *   2. метод, приводящий все строки в списке в верхний регистр
 *   3. метод возвращающий список квадратов всех уникальных элементов списка
 */

package lab7;

import java.util.*;

public class Streamer
{
    public static List<Integer> makeIntegerList()
    {
        Random random = new Random();
        List<Integer> numbers = new ArrayList<>();

        for (int i = 0; i < 10; ++i)
            numbers.add(random.nextInt(10));

        return numbers;
    }

    public static List<String> makeStringList()
    {
        List<String> strings = new ArrayList<>();

        strings.add("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
        strings.add("Sed interdum porta eros, eget vestibulum lectus hendrerit sit amet.");
        strings.add("Vestibulum semper ligula sed leo viverra, id elementum eros fermentum.");
        strings.add("Donec iaculis, ex quis efficitur rutrum, quam magna convallis enim, eget sagittis metus libero eu justo.");
        strings.add("Etiam tempus cursus augue id blandit.");

        return strings;
    }

    public static double getAvg(List<Integer> numbers)
    {
        double value = 0;
        OptionalDouble avg = numbers.stream().mapToDouble((number) -> number).average();
        try
        {
            value = avg.getAsDouble();
        }
        catch (NoSuchElementException error)
        {
           /*
            * Операция поиска элемента в объекте одного из контейнерных классов
            * завершилась неудачей (тип определен в пакете java.util).
            */
            System.out.println("Program trow exception: " + error.toString());
        }

        return value;
    }

    public static List<String> toUpperCase(List<String> strings)
    {
        List<String> uc_strings = new ArrayList<>();
        strings.stream().map(String::toUpperCase).forEach(uc_strings::add);
        return uc_strings;
    }

    public static List<Integer> uniqueSqr(List<Integer> numbers) {
        List<Integer> result_numbers = new ArrayList<>();

        numbers.stream().distinct().map(number -> number * number).forEach(result_numbers::add);

        return result_numbers;
    }

    public static void main(String[] args) {
        //===== A =====
        System.out.println("1. метод, возвращающий среднее значение списка целых чисел\n");
        System.out.println("AVG = " + getAvg(makeIntegerList()));
        System.out.println("============================================================\n");

        //===== B =====
        System.out.println("2. метод, приводящий все строки в списке в верхний регистр\n");
        toUpperCase(makeStringList()).stream().forEach(s -> System.out.println(s));
        System.out.println("============================================================\n");

        //===== C =====
        System.out.println("3. метод возвращающий список квадратов всех уникальных элементов списка\n");
        uniqueSqr(makeIntegerList()).stream().forEach(num -> System.out.print(num + " "));
        System.out.println("\n============================================================\n");
    }
}