/*
 * Дан массив без повторяющихся элементов. Перемешать его элементы случайным образом так,
 * чтобы каждый элемент оказался на новом месте.
 * Вывести исходный и получившийся массивы в консоль.
 */

package lab1;

import java.util.Random;

public class Shuffle
{
    static void printArray(int array[])
    {
        for (int j : array)
            System.out.print(j + " ");
        System.out.println("\n");
    }

    static void shuffleArray(int[] array)
    {
        Random random = new Random();

        for (int i = 0; i < array.length; ++i)
        {
            int j = random.nextInt(array.length - i);
            int temp = array[i];
            array[i] = array[j];
            array[j] = temp;
        }
    }

    public static void main(String[] args)
    {
        int[] array = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        printArray  (array);
        shuffleArray(array);
        printArray  (array);
    }
}