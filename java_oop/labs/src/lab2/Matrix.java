/*
 * Дана матрица. В каждой строке найти наибольший элемент. Из этих элементов найти наименьший и удалить ту строку,
 * которой он принадлежит. Вывести исходную и получившуюся матрицы в консоль.
 */

package lab2;

public class Matrix
{
    static void printMatrix(int[][] matrix)
    {
        for (int[] ints : matrix)
        {
            for (int j = 0; j < matrix[0].length; ++j)
                System.out.print(ints[j] + " ");
            System.out.println();
        }
    }

    static int findRow(int[][] matrix)
    {
        int[] maxArray = new int[matrix.length];
        int row = 0;

        for (int i = 0; i < matrix.length; ++i)
        {
            int max = 0;

            for (int j = 0; j < matrix[0].length; ++j)
                max = Math.max(max, matrix[i][j]);

            maxArray[i] = max;
        }

        int minOfMax = maxArray[0];

        for (int i = 0; i < maxArray.length; ++i)
        {
            if (maxArray[i] < minOfMax)
            {
                minOfMax = maxArray[i];
                row = i;
            }
        }

        for (int i : maxArray)
            minOfMax = Math.min(minOfMax, i);

        return row;
    }

    static int[][] deleteRow(int[][] matrix, int row)
    {
        int[][] newArray = new int[matrix.length - 1][matrix[0].length];

        int p = 0;
        for (int i = 0; i < matrix.length; ++i)
        {
            if (i == row)
                continue;

            int q = 0;
            for (int j = 0; j < matrix[0].length; ++j)
            {
                newArray[p][q] = matrix[i][j];
                ++q;
            }

            ++p;
        }

        return newArray;
    }

    public static void main(String[] args)
    {
        int[][] matrix = new int[][]{{ 1,  2, 33},
                                     { 4,  5, 6 },
                                     { 7,  8, 99}};

        printMatrix(matrix);
        System.out.println();
        printMatrix(deleteRow(matrix, findRow(matrix)));
    }
}