import java.util.Scanner;

class FactorialCache {
    private long[] mFactorials;

    public FactorialCache() {
        this(21);
    }

    public FactorialCache(int iSize) {
        if (iSize <= 0) {
            iSize = 21;
        }
        mFactorials = new long[iSize];
        buildCache();
    }

    public void setFactorials(long[] lFactorials) {
        if (lFactorials != null && lFactorials.length > 0) {
            mFactorials = lFactorials;
            buildCache();
        }
    }

    public long[] getFactorials() {
        return mFactorials;
    }

    public int getSize() {
        return mFactorials.length;
    }

    public long getFactorial(int iX) {
        if (iX < 0) {
            throw new IllegalArgumentException("value of x must be positive");
        }
        if (iX >= mFactorials.length) {
            throw new IllegalArgumentException("result will overflow");
        }
        return mFactorials[iX];
    }

    private void buildCache() {
        mFactorials[0] = 1L;
        for (int i = 1; i < mFactorials.length; i++) {
            mFactorials[i] = mFactorials[i - 1] * i;
        }
    }
}

interface IFactorialService {
    public long getFactorial(int iX);
    public int getMaxSupportedX();
}

class FactorialService implements IFactorialService {
    private FactorialCache mCache;

    public FactorialService() {
        this(new FactorialCache());
    }

    public FactorialService(FactorialCache oCache) {
        if (oCache == null) {
            oCache = new FactorialCache();
        }
        mCache = oCache;
    }

    public void setCache(FactorialCache oCache) {
        if (oCache != null) {
            mCache = oCache;
        }
    }

    public FactorialCache getCache() {
        return mCache;
    }

    @Override
    public long getFactorial(int iX) {
        return mCache.getFactorial(iX);
    }

    @Override
    public int getMaxSupportedX() {
        return mCache.getSize() - 1;
    }
}

public class Factorial {
    public static void main(String[] args) {
        Scanner oScanner = new Scanner(System.in);
        IFactorialService oService = new FactorialService();

        System.out.print("Enter x: ");
        int iX = oScanner.nextInt();

        try {
            long lAns = oService.getFactorial(iX);
            System.out.println("Factorial(" + iX + ") = " + lAns);
        } catch (IllegalArgumentException oEx) {
            System.out.println(oEx.getMessage());
        }

        oScanner.close();
    }
}
