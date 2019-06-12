import java.io.*;
import java.util.*;

public class Problem2 {
    /* Global Constant values */
    final static int PRIME_NUM = 101;
        
    final static int MAX_NAME_LEN = 10;
    final static int MAX_ADDR_LEN = 15;
    final static int MAX_DOB_LEN = 10;

    final static int CLASS_SIZE = 100;
    final static int TABLE_SIZE = 25;

    final static boolean SUCCESS = true;
    final static boolean FAILURE = false;

    final static boolean LOG_DEBUG = false;
    final static boolean LOG_INFO = true;
        
    final static boolean MENU_DRIVEN_PROGRAM = true;
    final boolean STATISTICAL_ANALYSIS_PROGRAM = false;
    final static int STDS_ENTRIES_INSTANT = 100;
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
    };
        
    /* Class: Search Result */
    class gc_SearchResult {
        public boolean                 m_bOpResult;
        public int                     m_iProbesCnt;
        public gc_Student_Record          m_oRecord;
        
        gc_SearchResult() {
        }
    };    
    
    /* Class: Contains the type of departments */
    class gc_Department {
       /*CS(0),
       ME(1),
       CE(2),
       EE(3);*/
       
       private int m_iDepartment;
       
       gc_Department() {
           m_iDepartment = 0;
       }
       
       public void m_fSetDepartment(String str) {
          if(str.equals("CS")) m_iDepartment = 0;
          else if(str.equals("ME")) m_iDepartment = 1;
          else if(str.equals("CE")) m_iDepartment = 2;
          else m_iDepartment = 3; /* if(str.equals("EE")) */ 
       }
       
       public String m_fGetDepartment() {
           if(m_iDepartment == 0) return "CS";
           else if(m_iDepartment == 1) return "ME";
           else if(m_iDepartment == 1) return "CE";
           else return "EE"; /* if(department == 1) */
       }
    }

    /* Class: Contains types of Gender of the person */
    class gc_Gender {
        /*
        MALE(0),
        FEMALE(1);
         */
        
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
    
    /* Class related for Student records */
    class gc_Student_Record {
        public int                     m_iRollNo;
        public String                 m_strAddress;
        public String                 m_strDOB;
        public gc_Department         m_oDepartment;
        public gc_Gender             m_oGender;
    }    

    /* Class for HASH TABLE */
    class gc_HashTable {
        /* Hash Table Entry */
        class c_HashRecord {
            int                     m_iKey;
            gc_Student_Record       m_oRecord;
            c_HashRecord            m_rNextEntry;
        }

        /* Represents every element in the Chain (linked list) */
        class c_NodeList {
            c_HashRecord         m_rRecord;
        }
        
        /* Array of HashEntry types */
        c_NodeList[]    m_arroSlotHeaders;
        gc_Pair         m_oHashFunc;
        int             m_iTableSize;
        
        /* Constructor */
        gc_HashTable(int iTableSize, gc_Pair ps_HashFuncParams) {
            m_iTableSize = iTableSize;
            
            /* Allocate Memory */
            m_arroSlotHeaders = new c_NodeList [iTableSize];
            for(int iter=0; iter<iTableSize; iter++)
            {
                m_arroSlotHeaders[iter] = new c_NodeList();
                m_arroSlotHeaders[iter].m_rRecord = null;
            }
            m_oHashFunc = new gc_Pair(ps_HashFuncParams.m_uiVal1, ps_HashFuncParams.m_uiVal2);
        }

        /* Insert record into the hash-table */
        boolean m_fInsert(int key, gc_Student_Record psRecord) {
            int uiSlot = ((m_oHashFunc.m_uiVal1 * key + m_oHashFunc.m_uiVal2) % PRIME_NUM) % m_iTableSize;

            // Chain is empty
            if(m_arroSlotHeaders[uiSlot].m_rRecord == null)
            {
               m_arroSlotHeaders[uiSlot].m_rRecord = new c_HashRecord();
               m_arroSlotHeaders[uiSlot].m_rRecord.m_iKey       = key;
               m_arroSlotHeaders[uiSlot].m_rRecord.m_oRecord    = psRecord;
               m_arroSlotHeaders[uiSlot].m_rRecord.m_rNextEntry = null;
            }
            else
            {
                // Check for Duplicate Record
                c_HashRecord current = m_arroSlotHeaders[uiSlot].m_rRecord;
                while(current != null)
                {
                    if(current.m_iKey == key)
                    {
                        if(LOG_DEBUG) 
                            System.out.println("Duplicate Key(" + key + ") found. Insert unsuccessful!");
                        return FAILURE;
                    }
                    else
                        current = current.m_rNextEntry;
                }

                // Append at front
                c_HashRecord temp = new c_HashRecord();
                temp.m_oRecord = psRecord;
                temp.m_iKey   = key; 

                current = m_arroSlotHeaders[uiSlot].m_rRecord;
                m_arroSlotHeaders[uiSlot].m_rRecord = temp;
                temp.m_rNextEntry = current;
            }
            return SUCCESS;
        }
        
        /* Search record using iKey in the hash-table */
        gc_SearchResult m_fSearch(int key)
        {
            gc_SearchResult oResult = new gc_SearchResult();
            oResult.m_iProbesCnt = 0;
            
            int uiSlot = ((m_oHashFunc.m_uiVal1 * key + m_oHashFunc.m_uiVal2) % PRIME_NUM) % m_iTableSize;
            oResult.m_iProbesCnt++;
              
            if( m_arroSlotHeaders[uiSlot].m_rRecord == null )
            {
                oResult.m_bOpResult = FAILURE;
                return oResult;
            }
            else
            {
                c_HashRecord current = m_arroSlotHeaders[uiSlot].m_rRecord;
                while(current != null)
                {
                	oResult.m_iProbesCnt++;
                    if(current.m_iKey == key)
                    {
                        oResult.m_bOpResult = SUCCESS;
                        oResult.m_oRecord = new gc_Student_Record();
                        oResult.m_oRecord = current.m_oRecord;
                        break;
                    }
                    else
                    	current = current.m_rNextEntry;
                }
                oResult.m_bOpResult = (current == null) ? FAILURE : SUCCESS;
            }
            return oResult;
        }

        /* Delete record using iKey in the hash-table */
        boolean m_fDelete(int iKey) 
        {
            int iSlot = ((m_oHashFunc.m_uiVal1 * iKey + m_oHashFunc.m_uiVal2) % PRIME_NUM) % m_iTableSize;
            
            if( m_arroSlotHeaders[iSlot].m_rRecord == null )
                return FAILURE;
            else
            {
                c_HashRecord current = m_arroSlotHeaders[iSlot].m_rRecord, prev = null;
                
                while(current != null)
                {
                    /* Record found */
                    if(current.m_iKey == iKey)
                    {
                        if(current == m_arroSlotHeaders[iSlot].m_rRecord)
                            m_arroSlotHeaders[iSlot].m_rRecord = m_arroSlotHeaders[iSlot].m_rRecord.m_rNextEntry;
                        else
                            prev.m_rNextEntry = current.m_rNextEntry;
                        
                        return SUCCESS;
                    }
                    else
                    {
                        prev = current;
                        current = current.m_rNextEntry;
                    }
                }

                if(current == null)
                    return FAILURE;
            }
            return SUCCESS;
        }
    }
    
    /* Global Student Database */
    public gc_Student_Record[] g_arrsStudentDB;

    /* Global Student Database Update: Read from the File */
    boolean m_fReadRecords() throws IOException
    {
        /* Allocating Memory */
        g_arrsStudentDB = new gc_Student_Record[CLASS_SIZE];
        for(int iter =0 ; iter <CLASS_SIZE; iter++ )
        {
            g_arrsStudentDB[iter] = new gc_Student_Record();
            g_arrsStudentDB[iter].m_oDepartment = new gc_Department();
            g_arrsStudentDB[iter].m_oGender = new gc_Gender(); 
        }
        
        /* Read from text file */
        FileReader in = null;
        try {
            in = new FileReader(fRecordFileName);
        } catch (Exception e) {
            System.out.println("Unable to open File.");
            if(LOG_DEBUG) e.printStackTrace(); 
            return FAILURE;
        }
        BufferedReader br = new BufferedReader(in);
        String read;
        int iRollAllotment = 0;

        while((read = br.readLine()) != null && iRollAllotment < CLASS_SIZE)  
        {
            String[] data = read.split(",");
            g_arrsStudentDB[iRollAllotment].m_iRollNo    = iRollAllotment + 1; 
            g_arrsStudentDB[iRollAllotment].m_strAddress = data[0];
            g_arrsStudentDB[iRollAllotment].m_strDOB     = data[1];
            g_arrsStudentDB[iRollAllotment].m_oDepartment.m_fSetDepartment(data[2]); 
            g_arrsStudentDB[iRollAllotment].m_oGender.SetGender(data[3]);
            iRollAllotment++;
        }

        // Debugging
        if(LOG_DEBUG) 
        {
            for(int iter=0; iter > iRollAllotment; iter++)
            {
                System.out.println(iter + "\n" + "RollNo: " + g_arrsStudentDB[iter].m_iRollNo);
                System.out.println("Address: " + g_arrsStudentDB[iter].m_strAddress);
                System.out.println("Date of Birth: " + g_arrsStudentDB[iter].m_strDOB);
                System.out.println("Department: " + g_arrsStudentDB[iter].m_oDepartment.m_fGetDepartment());
                System.out.println("Gender: " + g_arrsStudentDB[iter].m_oGender.GetGender());
            }
        }
        
        return SUCCESS;
    }
    
    /* Debugging: Used to store the keys */
    ArrayList< ArrayList<Integer> > arrListTempHashTable;

    /* Debugging: Print the temporary HashTable 
     * Sub-function: Pre-processing */
    void print_Temp_HashTable()
    {
        int i = 0;
        System.out.println("Hash Table Status: ");
        for(ArrayList<Integer> outIter : arrListTempHashTable) {
            System.out.print("\nSlot " + i +" (" + outIter.size() + "): ");
            for(Integer inIter : outIter) {
                System.out.print(inIter + " ");
            }
            i++;
        }
    }

    /* Retrieves the Hash Function parameters */
    void m_fRetreive_Hash_Params(
                int iTableSize,
                int iPrimeNum,
                gc_Pair ps_HashFunc)
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
            key = iter + 1;
            slot = ((a * key + b) % iPrimeNum) % iTableSize;
            arrListTempHashTable.get(slot).add(key);
        }

        ps_HashFunc.m_uiVal1 = a;
        ps_HashFunc.m_uiVal2 = b;

        if(LOG_INFO) 
            print_Temp_HashTable();
    }
    
    /* Menu driven program */
    void m_fCourse_spec_HashTable_Program(gc_HashTable pobj_HashTable) throws IOException
    {
        int uiSuccProbes = 0, uiUnSuccProbes = 0;
        int key = 10, choice = 10;        
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        
        System.out.println("");
        while(true) {
            /* Get User-operation */
            System.out.print("\n1 - Search\n2 - Insert\n3 - Delete\n4 - Exit\nYour Choice: ");
            try {
                choice = Integer.parseInt(br.readLine());
                if(choice == 1 || choice == 2 || choice == 3)
                {
                    System.out.print("Enter Roll No.: ");
                    key = Integer.parseInt(br.readLine());
                    if(key < 0 || key > CLASS_SIZE)
                        throw new IndexOutOfBoundsException();
                }
            } catch(IndexOutOfBoundsException formatExp) {
                System.out.println("Key should be within 1 to 100. Try again!");
                if(LOG_DEBUG)
                    formatExp.printStackTrace();
                continue;
            } catch(NumberFormatException formatExp){
                System.out.println("Please enter Integer only. Try again!");
                if(LOG_DEBUG)
                    formatExp.printStackTrace();
                continue;
            } catch (Exception e) {
                System.out.println("Please enter Integer only. Try again!");
                if(LOG_DEBUG)
                    e.printStackTrace();
                continue;
            }

            switch (choice) {

            case 1:
                gc_SearchResult rResult = pobj_HashTable.m_fSearch(key);
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
                if( SUCCESS != pobj_HashTable.m_fInsert(key, g_arrsStudentDB[key-1]) )
                    System.out.println("Record(" + key + ") entry failed. Duplicate record found!");
                else
                    System.out.println("Record(" + key + ") inserted successfully.");
                break;
                
            case 3:
                if( SUCCESS != pobj_HashTable.m_fDelete(key))
                    System.out.println("Record(" + key + ") not found");
                else
                    System.out.println("Record(" + key + ") Deleted!");
                break;
                
            case 4:
                System.out.println("Thank you.");
                return;
                
            default:
                System.out.println("Entered wrong choice. Please try again!");
            }
        }
    }

    /* Statistical Calculation of search times */
    void m_fHashTable_Statistical(gc_HashTable pobj_HashTable)
    {
        int iSuccProbes = 0, iUnSuccProbes = 0, noSuccProbes = 0, noUnSuccProbes = 0, iKey;
        
        /* Insert data into the Table */
        Random rand = new Random();
        int iStart = rand.nextInt(CLASS_SIZE-1) + 1;
        for(int iter=0; iter<STDS_ENTRIES_INSTANT; iter++)
        {
            iKey = ((iStart + iter) % CLASS_SIZE) + 1;
            if( SUCCESS != pobj_HashTable.m_fInsert(iKey, g_arrsStudentDB[iKey - 1]) )
                if(LOG_DEBUG) System.out.println("Record(" + iKey + ") entry failed. Duplicate record found!");
            else
                if(LOG_DEBUG) System.out.println("Record(" + iKey + ") inserted successfully.");
        }
        
        /* Run iteration for NUM_STATISTICAL_ITERATIONS times and calculate the number of probes */
        for(int iter=0;iter < NUM_STATISTICAL_ITERATIONS;iter++)
        {
            iKey = rand.nextInt(CLASS_SIZE-1) + 1;
        	
            gc_SearchResult rResult = pobj_HashTable.m_fSearch(iKey);
            if( rResult.m_bOpResult != SUCCESS )
            {
                iUnSuccProbes  = iUnSuccProbes + rResult.m_iProbesCnt;
                noUnSuccProbes++;
                if(LOG_DEBUG) System.out.println("Record(" + iKey + ") not present. Probes: " + rResult.m_iProbesCnt);
            }
            else
            {
            	int x = iSuccProbes;
                iSuccProbes = iSuccProbes + rResult.m_iProbesCnt;
                if(LOG_DEBUG) System.out.println("Record(" + iKey + ") Found. Probes: " + rResult.m_iProbesCnt);
            }
        }
        float alpha = STDS_ENTRIES_INSTANT/TABLE_SIZE;
        if(LOG_INFO) System.out.println("\n#Probes Succ: |" + iSuccProbes + "| Unsucc: |" + iUnSuccProbes + "|");
        System.out.print("\nAvg. No. Probes per operation:");
        if(noSuccProbes > 0) System.out.println("\nSuccessful Search - Experimental: " + ((float)iSuccProbes/(float)noSuccProbes) + " | Theoritical: " + (2 + ((float)alpha/(float)2) - ((float)alpha/(float)(2*STDS_ENTRIES_INSTANT))));
        if(noUnSuccProbes > 0) System.out.println("\nUnSuccessful Search - Experimental: " + ((float)iUnSuccProbes/(float)noUnSuccProbes) + " | Theoritical: " + (1 + (float)(alpha)));
        System.out.println("Overall - " + ((float)(iSuccProbes + iUnSuccProbes)/(float)NUM_STATISTICAL_ITERATIONS));
    }
    
    void Initialize() throws IOException
    {
        /* Read data records from file */ 
        if( SUCCESS != m_fReadRecords() ) {
            System.out.println("Error: Unable to read Data Records.");
            return;
        }

        /* Pre-processing 
        * For Static Keys, function calculates the parameters of Universal Hash Function(a,b) in 'p'-space */
        gc_Pair oHashFunc = new gc_Pair();
        m_fRetreive_Hash_Params(TABLE_SIZE, PRIME_NUM, oHashFunc);

        /* Allocate Hash Table */
        gc_HashTable pobj_HashTable = new gc_HashTable(TABLE_SIZE, oHashFunc);
        if(LOG_DEBUG)
            System.out.println("Selected Hash params: " + oHashFunc.m_uiVal1 + "|" + oHashFunc.m_uiVal2);
        
        /* Menu driven program */
        if(MENU_DRIVEN_PROGRAM)
            m_fCourse_spec_HashTable_Program(pobj_HashTable);
        
        /* Statistical Analysis */
        if(STATISTICAL_ANALYSIS_PROGRAM)
            m_fHashTable_Statistical(pobj_HashTable);
    }
    
    /* main - function*/
    public static void main(String[] args) throws IOException {
        Problem2 run = new Problem2();
        run.Initialize();
    }
}
