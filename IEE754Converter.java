import java.util.Scanner;

public class IEE754Converter {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.print("Inserisci il numero (con la virgola) da rappresentare in formato IEE-754 (I.E: 7,23): ");
        String input = scanner.nextLine();

        // Sostituire la virgola con un punto
        input = input.replace(',', '.');
        
        // Ora possiamo convertire l'input a double
        double numero = Double.parseDouble(input);

        // Step 1: Estrazione del segno
        int segno = (numero < 0) ? 1 : 0;
        numero = Math.abs(numero);

        // Step 2: Conversione in binario
        int parteIntera = (int) numero;
        double parteDecimale = numero - parteIntera;

        String parteInteraBin = Integer.toBinaryString(parteIntera);
        StringBuilder parteDecimaleBin = new StringBuilder();

        // Conversione della parte decimale in binario
        while (parteDecimale > 0 && parteDecimaleBin.length() < 23) {
            parteDecimale *= 2;
            if (parteDecimale >= 1) {
                parteDecimaleBin.append("1");
                parteDecimale -= 1;
            } else {
                parteDecimaleBin.append("0");
            }
        }

        String mantissa = parteInteraBin.substring(1) + parteDecimaleBin.toString();

        // Step 3: Calcolo dell'esponente
        int E = parteInteraBin.length() - 1;
        int bias = 127; // per singola precisione (32 bit)
        int esponente = E + bias;
        String esponenteBin = String.format("%8s", Integer.toBinaryString(esponente)).replace(' ', '0');

        // Step 4: Costruzione della rappresentazione finale
        String ieee754 = segno + esponenteBin + String.format("%-23s", mantissa).replace(' ', '0');

        // Assicura che la lunghezza della stringa binaria sia un multiplo di 4
        while (ieee754.length() % 4 != 0) {
            ieee754 += "0";
        }

        // Step 5: Conversione in esadecimale
        StringBuilder esadecimale = new StringBuilder();
        for (int i = 0; i < ieee754.length(); i += 4) {
            String quattroBit = ieee754.substring(i, i + 4);
            int decimale = Integer.parseInt(quattroBit, 2);
            esadecimale.append(Integer.toHexString(decimale).toUpperCase());
        }

        System.out.println("Rappresentazione in IEEE 754: " + ieee754);
        System.out.println("Rappresentazione esadecimale: " + esadecimale.toString());
    }
}
