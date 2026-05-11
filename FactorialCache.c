import java.util.Scanner;

class Factorial {

    static long[] fact = new long[21];

    public static long factorial(int x) {

        if (x < 0) {
            throw new IllegalArgumentException(
                "value of x must be positive"
            );
        }

        if (x >= fact.length) {
            throw new IllegalArgumentException(
                "result will overflow"
            );
        }

        fact[0] = 1;

        for (int i = 1; i <= x; i++) {
            fact[i] = fact[i - 1] * i;
        }

        return fact[x];
    }
}

public class Main {

    public static void main(String[] args) {

        Scanner sc = new Scanner(System.in);

        try {

            System.out.print("Enter a number: ");
            int n = sc.nextInt();

            long result = Factorial.factorial(n);

            System.out.println(
                "Factorial of " + n + " = " + result
            );
        }

        catch (IllegalArgumentException e) {

            System.out.println(
                "Exception: " + e.getMessage()
            );
        }

        sc.close();
    }
}
