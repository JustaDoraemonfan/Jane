import java.util.Scanner;

public class EmailValidation {

    public static void main(String[] args) {

        Scanner sc = new Scanner(System.in);

        try {

            System.out.print("Enter Email ID: ");
            String email = sc.nextLine();

            if (!(email.contains("@") && email.contains("."))) {

                throw new Exception("Invalid Email ID");
            }

            System.out.println("Valid Email ID");

        }

        catch (Exception e) {

            System.out.println("Exception: " + e.getMessage());
        }

        sc.close();
    }
}
