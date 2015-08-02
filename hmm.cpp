#include <iostream>
#include <vector>
using namespace std;


class HMM{
private:
    int N;		/* number of states;  Q={1,2,...,N} */
    int M; 		/* number of observation symbols; V={1,2,...,M}*/
    vector<vector<double>> A;	/* A[1..N][1..N]. a[i][j] is the transition prob
			   of going from state i at time t to state j
			   at time t+1 */
    vector<vector<double>> B;	/* B[1..N][1..M]. b[j][k] is the probability of
			   of observing symbol k in state j */
    vector<double> pi;	/* pi[1..N] pi[i] is the initial state distribution. */

public:
    /**
     * �����±궼�Ǵ�1��ʼ��
     */
    HMM(int n, int m, vector<vector<double>> AA, vector<vector<double>> BB, vector<double> PI ):
            N(n),M(m),A(AA),B(BB),pi(PI){}

    void __init__(int n, int m, vector<vector<double>> A, vector<vector<double>> B, vector<double> pi){
        this->N = n;
        this->M = m;

        this->A = A;
        this->B = B;
        this->pi = pi;
    }
    /**
     * ǰ���㷨��
     * �������ͳ��ѧϰ������P175ҳ
     * ��ǰ����ʣ�����������Ʒ�ģ�ͦˣ����嵽ʱ��t���ֹ۲�����Ϊo1,o2...ot����ʱ��t��״̬Ϊqi�ĸ���Ϊǰ�����
     *
     *  ͨ��ǰ����ʿ��Ե��Ƶ����ǰ����ʦ�_t(i)�͹۲����и���P(o|��)
     */
    void forward(int T, const vector<int> O,vector<vector<double>>& alpha, double* prob){
        /**
         * ��ʼ��
         */
        for(int i=1; i<=N; i++)
            alpha[1][i] = pi[i]*B[i][1];

        /**
         * ����
         */
        for(int t=1; t<T; t++){
            for(int i=1; i<=N; i++) {
                double sum = 0.0;
                for (int j = 1; j <= N; j++)
                    sum += alpha[t][j] * A[j][i];
                alpha[t+1][i] = sum * B[i][O[t+1]];
            }
        }

        /**
         * ��ֹ
         */
        *prob = 0.0;
        for(int i=1; i<= N; i++)
            *prob += alpha[T][i];
    }

    /**
     * �����㷨
     * ������ʣ�
     * ����������Ʒ�ģ�ͦˣ�����ʱ��t��״̬Ϊqi�ĸ����£���t+1��T���ֹ۲�����Ϊo_t+1,o_t+2...o_T
     *
     *  ͨ��ǰ����ʿ��Ե��Ƶ���ú�����ʦ�_t(i)�͹۲����и���P(o|��)

     */
    void backward(int T, vector<int> O, vector<vector<double>>& beta, double* prob){
        //��ʼ��������ʱ�̵�����״̬����Ϊ1
        for(int i=1; i<=N; i++)
            beta[T][1] = 1;

        /**
         * ���Ƽ���beta
         */
        for(int t=T-1; t>0; t--){
            for(int i=1; i<=N; i++){
                beta[t][i] = 0;
                for(int j=1; j<=N; j++){
                    beta[t][i] += A[i][j] *B[j][O[t+1]] * beta[t+1][j];
                }
            }
        }

        /**
         * �������
         */
        *prob = 0;
        for(int i=1; i<=N; i++){
            *prob += pi[i] * B[i][O[1]] * beta[1][i];
        }
    }

    void Viterbi(int T, vector<int> O, vector<vector<double>>& delta,
                 vector<vector<int>>& psi, vector<int>& path, double* prob){
        /**
         * ��ʼ��
         */

        for(int i=1; i<=N; i++){
            delta[1][i] = pi[i]*B[i][O[1]];
            psi[1][i] = 0.0;
        }

        /**
         * ����
         */
        for(int t=2; t<=T; t++){
            for(int i=1; i<=N; i++){
                double maxDelta = 0.0;
                int index = 1;
                for(int j=1; j<=N; j++){
                    if(maxDelta < delta[t-1][j] * A[j][i]){
                        maxDelta = delta[t-1][j] * A[j][i];
                        index = j;
                    }
                }
                delta[t][i] = maxDelta * B[i][O[t]];
                psi[t][i] = index;
            }
        }

        /**
         *  ��ֹ
         */

        *prob = 0;
        path[T] = 1;
        for(int i=1; i<=N; i++){
            if(*prob < delta[T][i]) {
                *prob = delta[T][i];
                path[T] = i;
            }
        }

        /**
         * ����·������
         */

        for(int t = T-1; t>0; t--){
            path[t] = psi[t+1][path[t+1]];
        }
    }
};


/**
 * ͳ��ѧϰ������ǰ����ʼ�������
 */
void testForward(){
    int n = 3;
    int m = 2;
    vector<vector<double>> A(n+1, vector<double>(n+1,0.0));
    vector<vector<double>> B(n+1, vector<double>(m+1,0.0));
    vector<double> pi(n+1,0.0);

    A[1][1] = A[2][2] = A[3][3] = 0.5;
    A[1][2] = A[2][3] = A[3][1] = 0.2;
    A[1][3] = A[2][1] = A[3][2] = 0.3;

    B[1][1] = B[1][2] = 0.5;
    B[2][1] = 0.4;
    B[2][2] = 0.6;
    B[3][1] = 0.7;
    B[3][2] = 0.3;

    pi = {0.0,0.2,0.4,0.4};

    double prob = 0.0;

    HMM hmm(n,m,A,B,pi);

    vector<int> O = {0,1,2,1};
    int T = 3;

    vector<vector<double>> alpha(T+1, vector<double>(n+1,0.0));
    hmm.forward(T, O, alpha, &prob);

    for(int i=1; i<=T; i++) {
        for (int j = 1; j <= n; j++)
            cout<<alpha[i][j]<<" ";
        cout<<endl;
    }
    cout<<prob<<endl;
}



void testViterbi(){
    int n = 3;
    int m = 2;
    vector<vector<double>> A(n+1, vector<double>(n+1,0.0));
    vector<vector<double>> B(n+1, vector<double>(m+1,0.0));
    vector<double> pi(n+1,0.0);

    A[1][1] = A[2][2] = A[3][3] = 0.5;
    A[1][2] = A[2][3] = A[3][1] = 0.2;
    A[1][3] = A[2][1] = A[3][2] = 0.3;

    B[1][1] = B[1][2] = 0.5;
    B[2][1] = 0.4;
    B[2][2] = 0.6;
    B[3][1] = 0.7;
    B[3][2] = 0.3;

    pi = {0.0,0.2,0.4,0.4};


    HMM hmm(n,m,A,B,pi);

    vector<int> O = {0,1,2,1};
    int T = 3;

    double prob = 0.0;

    vector<vector<double >> delta(T+1, vector<double>(n+1, 0.0));
    vector<vector<int >> psi(T+1, vector<int>(n+1, 0));

    vector<int> path (T+1, 0);

    hmm.Viterbi(T,O,delta,psi,path,&prob);

    for(int i=1; i<path.size(); i++)
        cout<<path.at(i)<<" ";
}


int main() {
//    testForward();
    testViterbi();
    return 0;
}