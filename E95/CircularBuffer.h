#pragma once

class CCircularBuffer
{
	int buffer_size = 5;
	float* sum = nullptr;
	float ave = 0.0f;
	int tail = 0;

public:

	CCircularBuffer() {
		sum = new float[buffer_size];
		Reset();
	}

	CCircularBuffer(int n) {
		buffer_size = n;
		sum = new float[buffer_size];
		Reset();
	}

	~CCircularBuffer()
	{
		if (sum) delete[] sum;
	}

private:
	void Set(float data) {
		if (tail >= buffer_size) tail = 0;
		ave -= sum[tail];
		sum[tail] = data;
		ave += sum[tail];
		tail++;
	}

	float Get() {
		return ave / buffer_size;
	}

public:
	float Read(float newVar) {
		Set(newVar);
		return Get();
	}

	void Reset() {
		for (short i = 0; i < buffer_size; i++)
		{
			sum[i] = 0.0f;
		}
	}
};
