import java.io.*;
import java.util.*;

public class Problem1 {
    /* Global Constant values */
    final static int PRIME_NUM = 101;

    final static int MAX_NAME_LEN = 10;
    final static int MAX_ADDR_LEN = 15;
    final static int MAX_DOB_LEN = 10;

    final static int MAX_STUDENTS = 100;
    final static int CLASS_SIZE = 25;
    final static int TABLE_SIZE = 15;

    final static boolean SUCCESS = true;
    final static boolean FAILURE = false;

    final static boolean LOG_INFO = true;
    final static boolean LOG_DEBUG = false;

    final static boolean MENU_DRIVEN_SEARCH_PROGRAM = true;
    final static boolean STATISTICAL_ANALYSIS_PROGRAM = false;
    final static int NUM_STATISTICAL_ITERATIONS = 2000;

    final static String fRecordFileName = "/home/michail/Java/records.txt";

    /* Class: Pair containing two-values */
    class gc_Pair {
        public int m_uiVal1, m_uiVal2;

        public gc_Pair(int val1, int val2) {
            m_uiVal1 = val1;
            m_uiVal2 = val2;
        }

        public gc_Pair() {
        }
    }

    /* Class: Triple containing three-values */
    class gc_Triple {
        int m_uiVal1, m_uiVal2, m_uiVal3;

        gc_Triple(int val1, int val2, int val3) 
        {
            m_uiVal1 = val1;
            m_uiVal2 = val2;
            m_uiVal3 = val3;
        }

        gc_Triple() {       
        }
    }

    /* Class: Result of search
     * Contain: Result Flag, No.Of Probes done, Record(if successful) */
    class gc_SearchResult {
        public boolean               m_bOpResult;
        public int                   m_iProbesCnt;
        public gc_Student_Record     m_oRecord;

        gc_SearchResult() {

        }
    }

    /* Class: Contains all the four departments */
    class gc_Department {
        /*CS(0),
          ME(1),
          CE(2),
          EE(3);*/

        private int m_iDepartment;

        gc_Department() {
            m_iDepartment = 0;
        }

        /* Set Department value */
        public void m_fSetDepartment(String str) {
            if(str.equals("CS")) m_iDepartment = 0;    
            else if(str.equals("ME")) m_iDepartment = 1;
            else if(str.equals("CE")) m_iDepartment = 2;
            else m_iDepartment = 3; /* if(str.equals("EE")) */ 
        }

        /* Get Department in String-format */
        public String m_fGetDepartment() {
            if(m_iDepartment == 0) return "CS";
            else if(m_iDepartment == 1) return "ME";
            else if(m_iDepartment == 1) return "CE";
            else return "EE"; /* if(department == 1) */
        }
    }

    /* Class: Contains types of Gender of the person */
    class gc_Gender {
        /* MALE(0),
           FEMALE(1); */

        private int m_iGender;

        gc_Gender() {
            m_iGender = 0;
        }

        gc_Gender(String str) {
            m_iGender = str.equals("MALE") || str.equals("M") ? 0 : 1;
        }

        public void SetGender(String str) {
            m_iGender = str.equals("MALE") || str.equals("M") ? 0 : 1;
        }

        public String GetGender() {
            return m_iGender == 0 ? "MALE" : "FEMALE";
        }
    }

    /* Class related to Student records */
    class gc_Student_Record {
        public int                   m_iRollNo;
        public String                m_strAddress;
        public String                m_strDOB;
        public gc_Department         m_oDepartment;
        public gc_Gender             m_oGender;
    }    

    /* Class for Perfect HASH TABLE */
    class gc_PerfectHashTable {

        /* Hashed Record Entry */
        class c_HashRecord {
            boolean                  m_bIsValidFlag;
            int                      m_iKey;
            gc_Student_Record        m_oRecord;
        }

        /* Secondary Hash Table */
        class c_SecHashTable {
            c_HashRecord[]  m_arroRecords;
            gc_Pair         m_oHashParams;
            int             m_iTableSize;
        }

        /* Primary Hash Table */
        c_SecHashTable[]    m_arroSecHashTable; /* Array of Secondary Hash Tables */
        gc_Pair             m_oHashParams;
        int                 m_iTableSize;

        /* Constructor */
        gc_PerfectHashTable(int iTableSize, gc_Pair rPriHashParams, gc_Triple[] arroSecHashParams) 
        {
            m_iTableSize = iTableSize;

            /* Allocated Primary Hash Table */
            m_arroSecHashTable = new c_SecHashTable[iTableSize];
            for(int iter=0; iter<iTableSize; iter++)
            {
                m_arroSecHashTable[iter] = new c_SecHashTable();
                m_arroSecHashTable[iter].m_arroRecords = null;
            }
            m_oHashParams = new gc_Pair(rPriHashParams.m_uiVal1, rPriHashParams.m_uiVal2);

            /* Allocated Secondary Hash Table */
            for(int iter=0; iter<iTableSize; iter++)
            {
                m_arroSecHashTable[iter].m_iTableSize  = arroSecHashParams[iter].m_uiVal3;
                m_arroSecHashTable[iter].m_oHashParams = new gc_Pair(arroSecHashParams[iter].m_uiVal1, arroSecHashParams[iter].m_uiVal2);
                if(m_arroSecHashTable[iter].m_iTableSize > 0)
                    m_arroSecHashTable[iter].m_arroRecords = new c_HashRecord[m_arroSecHashTable[iter].m_iTableSize];
                else
                    m_arroSecHashTable[iter].m_arroRecords = null;

                for(int iter2=0; iter2<m_arroSecHashTable[iter].m_iTableSize; iter2++)
                {
                    m_arroSecHashTable[iter].m_arroRecords[iter2] = new c_HashRecord(); 
                    m_arroSecHashTable[iter].m_arroRecords[iter2].m_bIsValidFlag = false;
                }
            }
        }

        /* Search record using iKey in the hash-table */
        gc_SearchResult m_fSearch(int iKey)
        {
            gc_SearchResult oResult = new gc_SearchResult();
            oResult.m_iProbesCnt = 2;
            int iPriSlot = ((m_oHashParams.m_uiVal1 * iKey + m_oHashParams.m_uiVal2) % PRIME_NUM) % m_iTableSize;

            if(m_arroSecHashTable[iPriSlot].m_arroRecords != null)
            {
                gc_Pair rSecHashFunc = m_arroSecHashTable[iPriSlot].m_oHashParams;
                int iSecTableSize    = m_arroSecHashTable[iPriSlot].m_iTableSize;
                int iSecSlot  = ((rSecHashFunc.m_uiVal1 * iKey + rSecHashFunc.m_uiVal2) % PRIME_NUM) % iSecTableSize;

                if(m_arroSecHashTable[iPriSlot].m_arroRecords[iSecSlot].m_bIsValidFlag == true &&
                        m_arroSecHashTable[iPriSlot].m_arroRecords[iSecSlot].m_oRecord.m_iRollNo == iKey)
                {
                    oResult.m_bOpResult = SUCCESS;
                    oResult.m_oRecord = new gc_Student_Record();
                    oResult.m_oRecord = m_arroSecHashTable[iPriSlot].m_arroRecords[iSecSlot].m_oRecord;                 
                    return oResult;
                }
                else // Record not present
                {
                    oResult.m_bOpResult = FAILURE;
                    return oResult;
                }
            }
            else // Record should not be present
            {
                oResult.m_bOpResult = FAILURE;
                return oResult;
            }
        }

        /* Insert record into the hash-table */
        boolean m_fInsert(int iKey, gc_Student_Record rRecord)
        {
            int iPriSlot = ((m_oHashParams.m_uiVal1 * iKey + m_oHashParams.m_uiVal2) % PRIME_NUM) % m_iTableSize;

            gc_Pair rSecHashFunc = m_arroSecHashTable[iPriSlot].m_oHashParams;
            int iSecTableSize    = m_arroSecHashTable[iPriSlot].m_iTableSize;
            int iSecSlot  = ((rSecHashFunc.m_uiVal1 * iKey + rSecHashFunc.m_uiVal2) % PRIME_NUM) % iSecTableSize;

            if(m_arroSecHashTable[iPriSlot].m_arroRecords[iSecSlot].m_bIsValidFlag == true)
            {
                if(m_arroSecHashTable[iPriSlot].m_arroRecords[iSecSlot].m_iKey == iKey )
                    System.out.println("Record(" + iKey + ") already inserted.");
                else
                    System.out.println("Error: Programming error. This scenario cannot occur!\n");
                return FAILURE;
            }
            else
            {
                m_arroSecHashTable[iPriSlot].m_arroRecords[iSecSlot].m_bIsValidFlag = true;
                m_arroSecHashTable[iPriSlot].m_arroRecords[iSecSlot].m_iKey = iKey;
                m_arroSecHashTable[iPriSlot].m_arroRecords[iSecSlot].m_oRecord = rRecord;             
                return SUCCESS;
            }
        }
    }

    /* Global Student Database */
    public gc_Student_Record[] g_arrsStudentDB;

    /* Global Student Database: Read from the File */
    boolean m_fReadRecords(
            Integer[] parri_RollCS,
            Integer[] parri_RollME,
            Integer[] parri_RollCE,
            Integer[] parri_RollEE) throws IOException
    {
        /* Allocating Memory */
        g_arrsStudentDB = new gc_Student_Record[MAX_STUDENTS];
        for(int iter =0 ; iter <MAX_STUDENTS; iter++ )
        {
            g_arrsStudentDB[iter] = new gc_Student_Record();
            g_arrsStudentDB[iter].m_oDepartment = new gc_Department();
            g_arrsStudentDB[iter].m_oGender = new gc_Gender(); 
        }

        /* Read from .txt file */
        FileReader in = null;
        try {
            in = new FileReader(fRecordFileName);
        } catch (Exception e) {
            if(LOG_DEBUG) e.printStackTrace(); 
            return FAILURE;
        }
        BufferedReader br = new BufferedReader(in);
        String read;
        int iRollAllotment = 0, iCSCnt = 0, iCECnt = 0, iMECnt = 0, iEECnt = 0;

        while((read = br.readLine()) != null && iRollAllotment < MAX_STUDENTS) 
        {
            String[] data = read.split(",");

            g_arrsStudentDB[iRollAllotment].m_iRollNo        = iRollAllotment + 1;
            g_arrsStudentDB[iRollAllotment].m_strAddress     = data[0];
            g_arrsStudentDB[iRollAllotment].m_strDOB         = data[1];
            g_arrsStudentDB[iRollAllotment].m_oGender.SetGender(data[2]);
            g_arrsStudentDB[iRollAllotment].m_oDepartment.m_fSetDepartment(data[3]);

            if(data[3].equals("CS")) parri_RollCS[iCSCnt++] = iRollAllotment + 1;
            else if(data[3].equals("ME")) parri_RollME[iMECnt++] = iRollAllotment + 1;
            else if(data[3].equals("CE")) parri_RollCE[iCECnt++] = iRollAllotment + 1;
            else if(data[3].equals("EE")) parri_RollEE[iEECnt++] = iRollAllotment + 1;
            else System.out.println("Programming Error!");
            iRollAllotment++;
        }

        /* Debugging */
        if(LOG_DEBUG)
        {
            for(int iter=0; iter < MAX_STUDENTS; iter++)
            {
                System.out.println("\n" + "RollNo: " + g_arrsStudentDB[iter].m_iRollNo);
                System.out.println("Address: " + g_arrsStudentDB[iter].m_strAddress);
                System.out.println("Date of Birth: " + g_arrsStudentDB[iter].m_strDOB);
                System.out.println("Department: " + g_arrsStudentDB[iter].m_oDepartment.m_fGetDepartment());
                System.out.println("Gender: " + g_arrsStudentDB[iter].m_oGender.GetGender());
            }
        }

        return SUCCESS;
    }

    ArrayList< ArrayList<Integer> > arrListTempHashTable;

    /* Debugging: Prints the HashTable 
     * Sub-function: Pre-processing */
    void print_Temp_HashTable(gc_Triple[] arroSecHashParams)
    {
        int iter = 0;
        gc_Triple rQuad;
        System.out.println("Perfect Hash Table Status: ");
        for(ArrayList<Integer> outIter : arrListTempHashTable) {
            rQuad = arroSecHashParams[iter];
            System.out.print("\nSlot " + iter +" : (" + rQuad.m_uiVal1 + ", " + rQuad.m_uiVal2 + ", " + rQuad.m_uiVal3 + ") " );
            for(Integer inIter : outIter)
                System.out.print(inIter + " ");
            iter++;
        }
        System.out.println("");
    }

    /* Retrieves the Secondary Hash Function parameters */
    void m_fRetreive_SecondaryHashParams(int iPrimeNum, gc_Triple[] arroSecHashParams)
    {
        /* Allocate Memory */
        for(int iter = 0; iter < TABLE_SIZE; iter++)
            arroSecHashParams[iter] = new gc_Triple();

        int a, b, iTableSize, iter2, iter3;
        boolean bCollisionFlag;

        for(int iter=0; iter<TABLE_SIZE; iter++)
        {
            while(true)
            {
                Random rand = new Random();
                a = rand.nextInt(iPrimeNum-1) + 1;
                while(a == 0)
                    a = rand.nextInt(iPrimeNum-1) + 1;
                b = rand.nextInt(iPrimeNum);

                bCollisionFlag = false;
                int iCollisions = arrListTempHashTable.get(iter).size();
                iTableSize = iCollisions * iCollisions;
                Integer[] arriKeySlots = new Integer[iCollisions];

                /* Get the slots w.r.t. each key using the 'selected' Hash-function*/
                iter2 = 0;
                for(Integer iKey : arrListTempHashTable.get(iter)) 
                {
                    arriKeySlots[iter2] = ((a*iKey + b) % iPrimeNum) % iTableSize;
                    iter2++;
                }

                /* Check whether any of the keys are mapped to the same slot */
                for(iter2=0; iter2 < iCollisions; iter2++)
                {
                    for(iter3 = iter2 + 1; iter3 < iCollisions; iter3++)
                    {
                        if(arriKeySlots[iter2] == arriKeySlots[iter3])
                        {
                            bCollisionFlag = true;
                            break;
                        }
                    }
                }

                if(bCollisionFlag == false) // Hash-function verified
                {
                    arroSecHashParams[iter].m_uiVal1 = a;
                    arroSecHashParams[iter].m_uiVal2 = b;
                    arroSecHashParams[iter].m_uiVal3 = (iCollisions * iCollisions);
                    break;
                }
            }
        }
        return;
    }

    /* Retrieves the Perfect Hash Function parameters */
    void m_fRetreive_Perfect_Hash_Params(
            Integer[] parri_Roll,
            int iTableSize,
            int iPrimeNum,
            gc_Pair ps_HashFunc,
            gc_Triple[] arroSecHashParams) 
    {
        /* Debugging: Initialize tempHashTable */
        arrListTempHashTable = new ArrayList< ArrayList<Integer> >(TABLE_SIZE);

        for(int iter=0; iter<TABLE_SIZE; iter++)
            arrListTempHashTable.add(new ArrayList<Integer>());

        /* Randomly choose a Universal hash function */
        int a, b, slot, key;
        Random rand = new Random(); 
        a = rand.nextInt(iPrimeNum-1) + 1;
        while(a == 0)
            a = rand.nextInt(iPrimeNum-1) + 1;
        b = rand.nextInt(iPrimeNum);

        /* Debugging : Store the possible collision keys into the ArrayList */
        for(int iter=0; iter<CLASS_SIZE; iter++)
        {
            key = parri_Roll[iter];
            slot = ((a * key + b) % iPrimeNum) % iTableSize;
            arrListTempHashTable.get(slot).add(key);
        }

        ps_HashFunc.m_uiVal1 = a;
        ps_HashFunc.m_uiVal2 = b;

        m_fRetreive_SecondaryHashParams(iPrimeNum, arroSecHashParams);

        /* Print the temporary Hash Table */
        print_Temp_HashTable(arroSecHashParams);
    }

    /* Menu driven search program */
    void m_fCourse_spec_HashTable_Search_Program(gc_PerfectHashTable pobj_PerfectHashTable) throws IOException
    {
        int uiSuccProbes = 0, uiUnSuccProbes = 0;
        int key = 10, choice = 9;

        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        String read;
        while(true) {
            System.out.print("\n1 - Search\n2 - Exit\nYour Choice: ");
            /* Get User-operation */
            try {
                choice = Integer.parseInt(br.readLine());
                if(choice == 1)
                {
                    System.out.print("Enter Roll No.: ");
                    key = Integer.parseInt(br.readLine());
                    if(key < 0 || key > 100)
                        throw new IndexOutOfBoundsException();
                }
            } catch(IndexOutOfBoundsException formatExp) {
                System.out.println("Key should be within 1 to 100. Try again!");
                if(LOG_DEBUG)
                    formatExp.printStackTrace();
                continue;
            } catch(NumberFormatException formatExp){
                if(LOG_DEBUG) formatExp.printStackTrace();
                System.out.println("Please enter Integer only. Try again!");
                continue;
            } catch (Exception e) {
                System.out.println("Please enter Integer only. Try again!");
                if(LOG_DEBUG) e.printStackTrace();
                continue;
            }

            switch (choice)
            {
                case 1:               
                    gc_SearchResult rResult = pobj_PerfectHashTable.m_fSearch(key);
                    if( rResult.m_bOpResult == FAILURE)
                    {
                        uiUnSuccProbes  = uiUnSuccProbes + rResult.m_iProbesCnt;
                        System.out.println("Record(" + key + ") not present. Probes: " + rResult.m_iProbesCnt);
                    }
                    else
                    {
                        uiSuccProbes  = uiSuccProbes + rResult.m_iProbesCnt;
                        System.out.println("Record(" + key + ") Found. Probes: " + rResult.m_iProbesCnt);
                        System.out.println("Roll No. : " + rResult.m_oRecord.m_iRollNo + "\nAddress: " + rResult.m_oRecord.m_strAddress
                                + "\nDate of Birth: " + rResult.m_oRecord.m_strDOB + "\nDepartment: " + rResult.m_oRecord.m_oDepartment.m_fGetDepartment()
                                + "\nGender: " + rResult.m_oRecord.m_oGender.GetGender());
                    }
                    break;

                case 2:
                    System.out.println("Thank you.");
                    return;

                default:
                    System.out.println("Entered wrong choice. Please try again!");
            }
        }
    }

    /* Statistical Calculation of search times */
    void m_fHashTable_Statistical(gc_PerfectHashTable pobj_PerfectHashTable, Integer[] parri_Roll)
    {
        int iSuccProbes = 0, iUnSuccProbes = 0, noSuccProbes = 0, noUnSuccProbes = 0, iKey;

        /* Run iteration for NUM_STATISTICAL_ITERATIONS times and calculate the number of probes */
        Random rand = new Random();
        for(int iter=0; iter < NUM_STATISTICAL_ITERATIONS; iter++)
        {
            iKey = parri_Roll[rand.nextInt(CLASS_SIZE - 1) + 1];

            gc_SearchResult rResult = pobj_PerfectHashTable.m_fSearch(iKey);
            if( rResult.m_bOpResult != SUCCESS )
            {
                iUnSuccProbes = iUnSuccProbes + rResult.m_iProbesCnt;
                if(LOG_DEBUG) System.out.println("Record(" + iKey + ") not present. Probes: " + rResult.m_iProbesCnt);
                noUnSuccProbes++;
            }
            else
            {
                iSuccProbes  = iSuccProbes + rResult.m_iProbesCnt;
                if(LOG_DEBUG) System.out.println("Record(" + iKey + ") Found. Probes: " + rResult.m_iProbesCnt);
                noSuccProbes++;
            }
        }

        if(LOG_INFO) System.out.println("\n#Probes Succ: |" + iSuccProbes + "| Unsucc: |" + iUnSuccProbes + "|");
        System.out.print("\nAvg. Probes per operation:");
        if(noSuccProbes > 0) System.out.println("\nSuccessful Search - Experimental: " + ((float)iSuccProbes/(float)noSuccProbes) + " | Theoritical: 2");
        if(noUnSuccProbes > 0) System.out.println("\nUnSuccessful Search - Experimental: " + ((float)iUnSuccProbes/(float)noUnSuccProbes) + " | Theoritical: 2");
        System.out.println("Overall - " + ((float)(iSuccProbes + iUnSuccProbes)/(float)NUM_STATISTICAL_ITERATIONS));
    }


    /* Initialize */
    void Initialize() throws IOException
    {
        /* Memory Allocation */
        Integer[] parri_RollCS = new Integer[CLASS_SIZE];
        Integer[] parri_RollME = new Integer[CLASS_SIZE];
        Integer[] parri_RollCE = new Integer[CLASS_SIZE];
        Integer[] parri_RollEE = new Integer[CLASS_SIZE];
        for(int iter = 0; iter < CLASS_SIZE; iter++)
        {
            parri_RollCS[iter] = new Integer(0);
            parri_RollME[iter] = new Integer(0);
            parri_RollCE[iter] = new Integer(0);
            parri_RollEE[iter] = new Integer(0);
        }

        /* Read data records from file */ 
        if( SUCCESS != m_fReadRecords(parri_RollCS, parri_RollME, parri_RollCE, parri_RollEE) ) {
            System.out.println("Error: Unable to read Data Records.");
            return;
        }

        /* Preprocessing For only one Department 'CS' Students only 
         * For Static Keys, function calculates the params of Universal Hash Function(a,b) in 'p'-space */
        gc_Pair oPriHashParams = new gc_Pair();
        gc_Triple[] arroSecHashParams = new gc_Triple[TABLE_SIZE];

        /* Retrieve parameters for the Perfect Hash Table */
        m_fRetreive_Perfect_Hash_Params(parri_RollCS, TABLE_SIZE, PRIME_NUM, oPriHashParams, arroSecHashParams);

        /* Allocate Hash Table */
        gc_PerfectHashTable pobj_PerfectHashTable = new gc_PerfectHashTable(TABLE_SIZE, oPriHashParams, arroSecHashParams);
        if(LOG_INFO)
            System.out.println("Selected Primary Hash params: " + oPriHashParams.m_uiVal1 + "|" + oPriHashParams.m_uiVal2);

        /* Insert Student records of only 'CS' department into the Perfect Hash Table */
        for(int iter = 0, iKey; iter < CLASS_SIZE; iter++)
        {
            iKey = parri_RollCS[iter];
            if( SUCCESS != pobj_PerfectHashTable.m_fInsert(iKey, g_arrsStudentDB[iKey - 1]) )
                if(LOG_DEBUG) System.out.println("Record(" + iKey + ") entry failed. Duplicate record found!");
                else
                    if(LOG_DEBUG) System.out.println("Record(" + iKey + ") inserted successfully.");
        }

        /* Menu driven Search program */
        if(MENU_DRIVEN_SEARCH_PROGRAM)
            m_fCourse_spec_HashTable_Search_Program(pobj_PerfectHashTable);

        /* Statistical Analysis */
        if(STATISTICAL_ANALYSIS_PROGRAM)
            m_fHashTable_Statistical(pobj_PerfectHashTable, parri_RollCS); 
    }

    /* main - function*/
    public static void main(String[] args) throws IOException {
        Problem1 run = new Problem1();
        run.Initialize();        
    }
}
