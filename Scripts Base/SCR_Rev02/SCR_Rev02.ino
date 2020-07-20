
#define MAX_FIFO        100               // Tamanho da FIFO 

long unsigned int const Maxms = 500;      // Define a frequência mínima gerada. FreMin = 1/(2*Maxms)
long unsigned int const Minms = 5;        // Define a frequência máxima gerada. FreMax = 1/(2*Minms)

int const CTL = 11;                       // Porta de saída para a frequência gerada
int const LED = 13;                       // LED indicativo
int const Frequency = A1;                 // Pino onde o potenciômetro para ajuste da frequência será ligado
int const Amplitude = A0;                 // Pino onde será feita a leitura na tensão na pele (saída do circuito)
bool level = 0;                           // Utilizado para controle da polaridade da onda quadrada gerada
long newms;                               // Controla a duração de cada 1/2 ciclo da onda quadrada juntamente com Fout
float baseLine;                           // Contém o valor de referência para o início da coleta do SCR

int  FIFO[MAX_FIFO];                      // Define FIFO software
int  index_in = 0;                        // Índice do vetor onde será armazenado o próximo dado na FIFO
int  index_out = 0;                       // Índice do vetor onde será lido o próximo dado na FIFO
int  nDataFIFO = 0;                       // Quantidade de dados na FIFO
bool FIFOempty = 1;                       // Indicador de FIFO vazia
int  TAM_FIFO;                            // Tamanho da FIFO 5 bytes

/**************************************************/
// Lê um dado da FIFO
int popFIFO()
{
  int  aux;
    
  if(nDataFIFO > 0)
  {
    aux = FIFO[index_out];
    index_out++;
    if(index_out == TAM_FIFO)
      index_out = 0;
    nDataFIFO--;
    return aux;
  }
  else
  {
    FIFOempty = 1;
    return 0;
  }
}

/****************************************************************************************************/
// Armazena um dado na FIFO
// Se a FIFO estiver cheira, sempre que uma nova amostra chega a mais antiga é descartada.
void pushFIFO(int dado)
{
  if(nDataFIFO == TAM_FIFO)
    popFIFO();
  FIFO[index_in] = dado;
  index_in++;
  if(nDataFIFO<TAM_FIFO)
    nDataFIFO++;  
  if(index_in == TAM_FIFO) 
    index_in = 0;
  FIFOempty = 0;
}

/****************************************************************************************************/
// Converte a amostra de 8 bits em um valor de tensão entre 0 e 5 volts
float convVolts(int sample)
{
  float Vout;

  Vout = ((float)sample*5.0)/1023.0;
  return(Vout);
}


/****************************************************************************************************/
// Calcula a média dos dados na FIFO. Importante para // atenuar ruídos durante a leitura da tensão 
// na pele. Retorna o valor médio em volts das amostras armazenadas na FIFO
float calcPoint()
{
  int med = 0;
  float volts;
  for (int i=0;i<TAM_FIFO;i++)
  {
    med = FIFO[i]+med;
  }
  med = med / TAM_FIFO;
  volts = convVolts(med);
  return(volts);
}


/****************************************************************************************************/
// Lê uma amostra da tensão na pele
int leSample()
{
  int Vout;
  
  Vout = analogRead(Amplitude);
  return(Vout);
}


/****************************************************************************************************/
// Ajusta a frequência da onda quadrada e lê uma amostra da tensão na pele antes de inverter a
// polaridade da onda quadrada
void freqAdjLeSCR()
{
  long unsigned int Fout;
  int VlFreq, sample;
  
  VlFreq = analogRead(Frequency);
  Fout = map(VlFreq,5,1015,Maxms,Minms);
  //Serial.print(Fout);Serial.print("\t");Serial.print(VlFreq);Serial.println("\t");

  if (Fout>Maxms)
  {
    digitalWrite(CTL,LOW);
    sample = leSample();
    pushFIFO(sample);
  }
  else  if (Fout<Minms)
        {
          digitalWrite(CTL,HIGH);
          sample = leSample();
          pushFIFO(sample);
        }  
        else if (millis()>newms)
             {
                if (Fout<Minms) Fout = Minms;
                if (Fout>Maxms) Fout = Maxms;
                level = 1 - level;
                if (level)
                {
                  sample = leSample();  
                  pushFIFO(sample);
                }
                digitalWrite(CTL,level);
                newms = millis() + Fout;                              
             }
}

void setup() 
{
  Serial.begin(115200);
  pinMode(CTL, OUTPUT);             // Frequency pin output
  pinMode(LED,OUTPUT);
  pinMode(Frequency, INPUT);        // Output frequency control
  pinMode(Amplitude, INPUT);        // Output amplitude control
  newms = millis();
  digitalWrite(LED,LOW);
  TAM_FIFO  = 100;                  // Número de amostras para o cálculo da baseLine
  nDataFIFO = 0;
  while(nDataFIFO<TAM_FIFO)         // Enche a FIFO com amostras da tensão na pele para
  {                                 // o cálculo da baseLine
    freqAdjLeSCR(); 
  }  
  baseLine = calcPoint();
  nDataFIFO = 0;
  TAM_FIFO = 16;                    // Com este valor, cada amostra da tensão na pele é resultante da média de 16 amostras
  digitalWrite(LED,HIGH);           // Acende o LED indicando que o cálculo da baseLine foi concluído
//  Serial.println(baseLine);
}

void loop() 
{
  float amp, ResSkin,CondSkin;  // Amplitude, Resistência e Condutância da pele
  float freq;
  int sample;

  freqAdjLeSCR();               // Ajusta a frequência e lê a próxima amostra da tensão na pele
  amp = calcPoint()-2.5;        // Calcula a média das últimas TAM_FIFO amostras. 2,5 é o valor da tensão de referência
  ResSkin = 400000*amp;         // Converte tensão para resistência a partir do valor da corrente previamente definida
  CondSkin = (1/ResSkin)*1000000; // Converte resistência em condutância (micro siemens - S)
    {
      Serial.print(ResSkin);Serial.print("\t");/*Serial.print(amp);Serial.print("\t");*/Serial.println(CondSkin);
    }
}
