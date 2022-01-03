#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
	char filename[30];
	int usedmethod;
	int linenum=0;
	int pageframenum=0; //값이 최소 1~ 최대 4
	char temp[256]; //최소 1개 ~ 최대 30개
	char *ptr; //strtok를 사용할 때 필요한 포인터
	int index=0;
	int prs[30]; //prs값은 최대 30개
	int prsnum=0;	
	FILE* fp;

	memset(filename,0,sizeof(char)*30);
	memset(prs,-1,sizeof(int)*30);

	/*	
	for(i=0;i<30;i++){
		printf("[%d]:%d\n",i,prs[i]);
	}
	*/

	printf("Filename : ");
	scanf("%s", filename);
	printf("\n");
	
	printf("Used method (OPT(1) FIFO(2) LRU(3) Second-Chance(4)) : ");
	scanf("%d", &usedmethod);
	
	if((fp = fopen(filename, "r")) == NULL){
		printf("file open error\n");
		return -1;
	}

	while(!feof(fp)){
		memset(temp, 0, sizeof(temp));
		fgets(temp,sizeof(temp),fp);
		temp[strlen(temp)-1] = '\0'; //마지막 개행 삭제

		if(linenum == 0){
			pageframenum = atoi(temp);
			//printf("pageframenum : %d\n", pageframenum);
		}
		else if(linenum ==1){
			printf("page reference string : %s\n\n", temp);
			ptr = strtok(temp, " ");

			while(ptr != NULL){
				prs[index] = atoi(ptr); //자른 문자열을 인트로 변환해 저장
				ptr = strtok(NULL, " ");
				index++;
				prsnum = index; //늘어난 인덱스 개수만큼만 포문을 돌릴 예정임
			}
		}
		linenum++;
	}
	
	int pageframe[pageframenum];

	memset(pageframe, -1, sizeof(int)*pageframenum);

	printf("\tframe\t");
	for(int i=0; i<pageframenum;i++){
		printf("%d\t",i+1);
	}
	printf("page fault\n");
	printf("time\n");

	if(usedmethod == 1){
		int pagefaultOPT=0;
		int check=0;
		int checkEmptyOPT=0;
	
		for (int i =0; i<prsnum; i++){
			printf("%d\t\t",i+1);

			check = 0;
			checkEmptyOPT = 0;

			//pageframe배열에서 같은 값이 있는지 찾아봄 
			for(int j=0; j<pageframenum; j++){ //pageframe배열에서 같은 값이 있는지 찾아봄 
				
				if(prs[i] == pageframe[j]){ //있으면 page fault 아니고 출력 아무것도 안함
					check = 1;

					//프린트해주기 
					for(int c=0; c<pageframenum; c++){
						if(pageframe[c] != -1){
							printf("%d", pageframe[c]);
						}
						printf("\t");
					}
				}
			}

			//check가 되지 않은 pagefault가 발생하는 경우가 여기로 옴
			if(!check){ //check가 안된 즉, pagefault가 난 친구들이 여기로 옴
				pagefaultOPT++;

				//-1인 자리가 있는지 확인하고 있으면 그 자리에 바로 넣어주고
				//프린트문 찍어주고 바로 나오면 됨
				for(int j=0; j<pageframenum; j++){ //-1인 자리가 있는지 확인 
					if(pageframe[j] == -1){ //-1인 자리가 있으면 그 자리에 바로 넣어주고 
						pageframe[j] = prs[i];
						checkEmptyOPT = 1; //-1인 비어있는 자리가 있어서 이미 처리했음을 의미

						//값을 출력해줌
						for(int k=0; k<pageframenum; k++){
							if(pageframe[k] != -1){ //-1이 아닌 값만 출력해줌
								printf("%d", pageframe[k]);
							}
							printf("\t"); //-1이던 아니던 탭은 한번씩 해줘야함 
						}
						//빠져나감 
						break; 
					}
				}

				//-1(비어있는 자리)가 없어서 원래 있던 놈들을 쫒아내고 넣어야 하는 경우
				if(!checkEmptyOPT){
					int count=0;
					int checkcount[pageframenum];
					int replaceIndex = -1;
					
					memset(checkcount,0,sizeof(int)*pageframenum);

					for(int j=i+1; j<prsnum; j++){
						for(int k=0;k<pageframenum;k++){
							if(prs[j] == pageframe[k]){
								if(checkcount[k]==0){
									checkcount[k]=1;
									count++;
									
									if(count==pageframenum){
										replaceIndex = k;
										break;
									}
								}
							}
						}
					}

					if(replaceIndex == -1){
						for(int j=0; j<pageframenum; j++){
							if(checkcount[j] ==0){
								replaceIndex = j;
								break;
							}
						}
					}
					
					pageframe[replaceIndex] = prs[i];
					
					//다 바꿔줬으니깐 출력해줌
					for(int j=0; j<pageframenum; j++){
						printf("%d\t", pageframe[j]);
					}
				}

				printf("F");
			}

			printf("\n");
		}

		printf("Number of page faults : %d times\n", pagefaultOPT);		
	}
	
	else if(usedmethod == 2){
		int pagefaultFIFO=0;
		int check;
		int age[pageframenum];
		int checkEmptyFIFO=0;

		memset(age,0,sizeof(int)*pageframenum);

		for(int i=0; i<prsnum; i++){
			printf("%d\t\t", i+1);

			check = 0;
			checkEmptyFIFO =0;

			//일단은 page fault인지 아닌지 체크해야 함
			//아래는 페이지폴트가 아닌 경우임
			for(int j=0; j<pageframenum; j++){
				if(prs[i] == pageframe[j]){
					check=1;

					//프린트해주고 age도 증가시켜 줘야함
					for(int k=0; k<pageframenum; k++){
						if(pageframe[k] != -1){
							age[k]++;
							printf("%d", pageframe[k]);
						}
						printf("\t");
					}
				}
			}

			//페이지폴트이면
			//경우는 두가지임
			//1. pageframe이 -1로 돼있어서 거기에다가 넣어주는 것,, 이때도 age++해야함 
			//2. -1이 없으면 즉 비어있는 페이지없으면 age가 가장 큰 것으로 교체해주고 나머지는 age++

			if(!check){ //페이지폴트인 경우
				pagefaultFIFO++;

				//1번 경우
				for(int j=0; j<pageframenum; j++){
					if(pageframe[j] == -1){
						pageframe[j] = prs[i];
						checkEmptyFIFO=1;

						for(int k=0; k<pageframenum; k++){
							if(pageframe[k] != -1){
								age[k]++;
								printf("%d", pageframe[k]);
							}
							printf("\t");
						}
						break;
					}
				}
				
				//2번 경우
				if(!checkEmptyFIFO){
					//가장 큰 age를 찾아줌
					int max = age[0];
					int maxindex = 0; //맨 처음 인덱스를 가리키고 있음

					for(int j=0; j<pageframenum; j++){
						if(age[j] > max){
							max = age[j];
							maxindex = j;
						}
					}

					//가장 큰 age값을 가진 maxindex로 교체해줌
					//교체를 해준 애를 포함해서 모든 값을 age++해줌 
					//교체를 해준 애는 age값을 초기화해줌 
					pageframe[maxindex] = prs[i];
					age[maxindex] =0;

					for(int j=0; j<pageframenum; j++){
						age[j]++; 
						printf("%d\t", pageframe[j]);
					}
				}

				printf("F");
			}
			printf("\n");
		}
		printf("Number of page faults : %d times\n", pagefaultFIFO);
	}
	
	else if(usedmethod == 3){
		int pagefaultLRU =0;
		int check=0;
		int checkEmptyLRU=0;

		for (int i =0; i<prsnum; i++){
			printf("%d\t\t",i+1);
			
			check = 0;
			checkEmptyLRU =0;
			
			//pageframe배열에서 같은 값이 있는지 찾아봄 
			for(int j=0; j<pageframenum; j++){ //pageframe배열에서 같은 값이 있는지 찾아봄 
				
				if(prs[i] == pageframe[j]){ //있으면 page fault 아니고 출력 아무것도 안함
					check = 1;
					//printf("[%d]이미 있는 경우\n", i);

					//프린트해주기 
					for(int c=0; c<pageframenum; c++){
						if(pageframe[c] != -1){
							printf("%d", pageframe[c]);
						}
						printf("\t");
					}
				}
			}
			
			//check가 되지 않은 pagefault가 발생하는 경우가 여기로 옴
			if(!check){ //check가 안된 즉, pagefault가 난 친구들이 여기로 옴
				pagefaultLRU++;

				//-1인 자리가 있는지 확인하고 있으면 그 자리에 바로 넣어주고
				//프린트문 찍어주고 바로 나오면 됨
				for(int j=0; j<pageframenum; j++){ //-1인 자리가 있는지 확인 
					if(pageframe[j] == -1){ //-1인 자리가 있으면 그 자리에 바로 넣어주고 
						pageframe[j] = prs[i];
						checkEmptyLRU = 1; //-1인 비어있는 자리가 있어서 이미 처리했음을 의미

						//값을 출력해줌
						for(int k=0; k<pageframenum; k++){
							if(pageframe[k] != -1){ //-1이 아닌 값만 출력해줌
								printf("%d", pageframe[k]);
							}
							printf("\t"); //-1이던 아니던 탭은 한번씩 해줘야함 
						}
						//빠져나감 
						break; 
					}
				}
				
				//비어있는 자리가 없어서 원래 있던 놈을 쫓아내고 넣는 경우
				if(!checkEmptyLRU){
					int count=0;
					int checkcount[pageframenum];
					int replaceIndex=-1;
					
					memset(checkcount, 0, sizeof(int)*pageframenum);

					//앞에 페이지들을 확인해야함 
					//확인을 하다가 마지막놈을 count를 하다가 conut-1이랑 같은 값이 되면
					//그 놈을 replaceindex로 결정함
					
					for(int j=i-1;j>-1;j--){
						for(int k=0;k<pageframenum;k++){
							if(pageframe[k]==prs[j]) {
								if(checkcount[k]==0){ //체크가 안됐을 때만 count를 해줌 
									checkcount[k]=1; //확인이 됐음을 체크함 
									count++;

									if(count==pageframenum){
										replaceIndex = k;
										break;
									}
								}
							}	
						}
					}
					pageframe[replaceIndex] = prs[i];

					for(int j=0; j<pageframenum; j++){
						printf("%d\t", pageframe[j]);
					}
				}
				printf("F");
			}
			printf("\n");
		}
		printf("Number of page faults : %d times\n", pagefaultLRU);
	}
	else if(usedmethod == 4){
		int pagefaultSC=0;
		int checkEmptySC=0;
		int check=0;
		int hit[pageframenum];
		int victim=-1;
		
		// -1페이지가 있을 때 victim을 검사함 
		// victim이 -1이었으면 가장 먼저 들어온 놈임을 알 수 있음
		// 그럼 그 값으로 victim이 가리키게 함 
		// 페이지 폴트가 난 경우 victim이 가리키는 놈을 방출시켜야함 
		// 이때 hit bit이 1이면 그 다음놈이 가리키게 함

		memset(hit,0,sizeof(int)*pageframenum);
		memset(hit,0,sizeof(int)*pageframenum);

		for(int i=0; i<prsnum; i++){
			printf("%d\t\t", i+1);

			check=0;
			checkEmptySC=0;

			//페이지폴트가 아닌 경우임
			//hit일때 hit bit 1로 해줘야함 
			for(int j=0; j<pageframenum; j++){
				if(prs[i] == pageframe[j]){
					check=1;
					hit[j]=1;

					//프린트해주고 hit bit를 1로 해줌
					for(int k=0; k<pageframenum; k++){
						if(pageframe[k] != -1){
							printf("%d", pageframe[k]);
						}
						printf("\t");
					}
				}
			}

			//페이지폴트이면
			//경우는 두가지임
			//1. pageframe이 -1로 돼있어서 거기에다가 넣어줌 그리고 victim이 -1이었으면
			//현재 인덱스 값을 넣어줘야 함 
			//2. -1이 없으면 즉 비어있는 프레임이 없으면 victim이 가리키는 곳에다가 넣음
			//근데 hit검사를 해서 1이 아니어야지 넣고 1이면 0인 값이 나올 때까지
			//1->0으로 바꿔줘야함 

			if(!check){ //페이지폴트인 경우
				pagefaultSC++;

				//1번 경우
				for(int j=0; j<pageframenum; j++){
					if(pageframe[j] == -1){
						pageframe[j] = prs[i];
						checkEmptySC=1;
						if(victim==-1){
							victim = j;
						}

						for(int k=0; k<pageframenum; k++){
							if(pageframe[k] != -1){
								printf("%d", pageframe[k]);
							}
							printf("\t");
						}
						break;
					}
				}

				//2번의 경우
				if(!checkEmptySC){
					//printf("여기는오니?");
					if(hit[victim]){
						while(1){ //hit bit가 0일 때까지 찾음 
							if(!hit[victim]) break;
							hit[victim] =0;
							//printf("여기안와?");
							victim = (victim+1) % pageframenum;
						}
					}
					//hit bit 0인 것을 찾고 나서 
					pageframe[victim] = prs[i];

					//그리고 그 다음 순서로 넘겨줌
					victim = (victim+1)%pageframenum;

					for(int j=0;j<pageframenum;j++){
						printf("%d\t", pageframe[j]);
					}
				}
				printf("F");
			}
			printf("\n");
		}
		printf("Number of page faults : %d times\n", pagefaultSC);
	}
	else{
		printf("Enter number among 1~4\n");
	}
	
	return 0;
}
