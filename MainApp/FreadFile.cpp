#include "FreadFile.h"

void FreadFile::readTemp()
{
	int isReadPoint = 0;//是否读到小数点

	int digitNumFrontPoint = 0;//小数点前有几位数
	int isMinusFrontPoint = 0;//小数点前是否有负号

	int digitNumBehindPoint = 0;//小数点后有几位

	double temporaryValue = 0;//临时数值

	int digitSequence = 0;//目前读到的序号

	for (int i=0; i < num; i++)
	{
		//cout<<i<<' ';
		if (temp[i-1] == 'z')
		{
			
			int j = i;
			while(temp[j] != NULL )
			{
				//cout<<j<<' ';
				//cout<<temp[j];//输出全部文本
				if (temp[j] != ' ' && temp[j] != '\n' && (temp[j-1] == ' ' || temp[j-1] == '\n'))//判断读取到数据
				{
					isReadPoint = 0;//是否读到小数点

					digitNumFrontPoint = 0;//小数点前有几位数
					isMinusFrontPoint = 0;//小数点前是否有负号

					digitNumBehindPoint = 0;//小数点后有几位

					temporaryValue = 0;//临时数值

					digitSequence = j + digitNumFrontPoint + isReadPoint + digitNumBehindPoint;

					while (temp[digitSequence] != ' ' && temp[digitSequence] != '\n')//统计单个数据位数
					{
						//cout<<temp[digitSequence]<<endl;
						if (temp[digitSequence] == '.')//是否读到小数点
						{
							isReadPoint = 1;
						}
						else if (temp[digitSequence] == '-')
						{
							isMinusFrontPoint = 1;
						}
						else if (isReadPoint == 0)//如果没有读到小数点
						{
							digitNumFrontPoint++;//小数点前位数数量加1
						}
						else
						{
							digitNumBehindPoint++;
						}
						digitSequence = j + isMinusFrontPoint + digitNumFrontPoint + isReadPoint + digitNumBehindPoint;
					}

					if (isMinusFrontPoint == 1)//如果有负号
					{
						//整数部分
						for (int k = 0; k < digitNumFrontPoint; k++)
						{
							temporaryValue = temporaryValue + (temp[j+1+k]-48) * (pow((double)10,digitNumFrontPoint-k-1));
						}

						//小数部分
						if (isReadPoint == 1)
						{
							for (int l = 0; l < digitNumBehindPoint; l++)
							{
								temporaryValue = temporaryValue + (temp[j+1+digitNumFrontPoint+1+l]-48) * (pow((double)10,-1 * (l+1)));
							}
						}

						temporaryValue *= -1;
					}
					else
					{
						//整数部分
						for (int k = 0; k < digitNumFrontPoint; k++)
						{
							//cout<<digitNumFrontPoint<<endl;
							//cout<<temp[j+k]-48<<endl;
							//cout<<(pow((double)10,digitNumFrontPoint-k-1))<<endl;

							temporaryValue = temporaryValue + (temp[j+k]-48) * (pow((double)10,digitNumFrontPoint-k-1));

							//cout<<temporaryValue<<endl;
						}

						//小数部分
						if (isReadPoint == 1)
						{
							for (int l = 0; l < digitNumBehindPoint; l++)
							{
								temporaryValue = temporaryValue + (temp[j+digitNumFrontPoint+1+l]-48) * (pow((double)10,-1 * (l+1)));
							}
						}
					}
				double *ptemporaryNum = new double;
				temporaryNum.push_back(*ptemporaryNum);

				temporaryNum[temporaryNum.size()-1] = temporaryValue;

				//cout<<temporaryNum[temporaryNum.size()-1]<<endl;

				}
				j++;
			}

			if (temp[j] == NULL)
			{
				break;
			}
		}
	}

	//for (int i = 0; i < temporaryNum.size(); i++)
	//{
	//	cout<<temporaryNum[i]<<endl;
	//}

	int xNum = 0;
	int yNum = 0;
	int zNum = 0;

	for (int i = 1; i < temporaryNum.size(); i=i+4)
	{
		PointData *pPointData = new PointData;
		m_PointData.push_back(*pPointData);

		m_PointData[xNum].Position.x = temporaryNum[i];

		//cout<<m_PointData[xNum].Position.x<<endl;

		xNum++;
	}

	for (int i = 2; i < temporaryNum.size(); i=i+4)
	{
		m_PointData[yNum].Position.y = temporaryNum[i];

		//cout<<m_PointData[yNum].Position.y<<endl;

		yNum++;
	}

	for (int i = 3; i < temporaryNum.size(); i=i+4)
	{
		m_PointData[zNum].Position.z = temporaryNum[i];

		//cout<<m_PointData[zNum].Position.z<<endl;

		zNum++;
	}

	
}