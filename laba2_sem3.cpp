#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
using namespace std;


const int MIN_MERGE = 64;

int calcMinRun(int n) {
    int r = 0;
    while (n >= MIN_MERGE) {
        r |= (n & 1);
        n >>= 1;
    }
    return n + r;
}

void insertionSort(std::vector<int>& arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = arr[i];
        int j = i - 1;

        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void merge(std::vector<int>& arr, int l, int m, int r) {
    int len1 = m - l + 1, len2 = r - m;
    std::vector<int> left(len1), right(len2);

    for (int i = 0; i < len1; i++)
        left[i] = arr[l + i];
    for (int i = 0; i < len2; i++)
        right[i] = arr[m + 1 + i];

    int i = 0, j = 0, k = l;

    while (i < len1 && j < len2) {
        if (left[i] <= right[j]) {
            arr[k] = left[i];
            i++;
        }
        else {
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    while (i < len1) {
        arr[k] = left[i];
        i++;
        k++;
    }

    while (j < len2) {
        arr[k] = right[j];
        j++;
        k++;
    }
}

int gallopRight(const std::vector<int>& arr, int key, int base, int len, int hint) {
    int lastOfs = 0;
    int ofs = 1;

    if (key > arr[base + hint]) {
        int maxOfs = len - hint;
        while (ofs < maxOfs && key > arr[base + hint + ofs]) {
            lastOfs = ofs;
            ofs = (ofs << 1) + 1;
            if (ofs <= 0) ofs = maxOfs;
        }
        if (ofs > maxOfs) ofs = maxOfs;

        lastOfs += hint;
        ofs += hint;
    }
    else {
        int maxOfs = hint + 1;
        while (ofs < maxOfs && key <= arr[base + hint - ofs]) {
            lastOfs = ofs;
            ofs = (ofs << 1) + 1;
            if (ofs <= 0) ofs = maxOfs;
        }
        if (ofs > maxOfs) ofs = maxOfs;

        int tmp = lastOfs;
        lastOfs = hint - ofs;
        ofs = hint - tmp;
    }

    lastOfs++;

    while (lastOfs < ofs) {
        int m = lastOfs + ((ofs - lastOfs) >> 1);

        if (key > arr[base + m])
            lastOfs = m + 1;
        else
            ofs = m;
    }
    return ofs;
}

void mergeWithGallop(std::vector<int>& arr, int l, int m, int r) {
    int len1 = m - l + 1, len2 = r - m;
    std::vector<int> left(len1), right(len2);

    for (int i = 0; i < len1; i++)
        left[i] = arr[l + i];
    for (int i = 0; i < len2; i++)
        right[i] = arr[m + 1 + i];

    int i = 0, j = 0, k = l;
    int gallopThreshold = 7;
    int gallopCount = 0;

    while (i < len1 && j < len2) {
        if (left[i] <= right[j]) {
            arr[k++] = left[i++];
            gallopCount = (gallopCount > 0) ? gallopCount - 1 : 0;
        }
        else {
            arr[k++] = right[j++];
            gallopCount = (gallopCount > 0) ? gallopCount - 1 : 0;
        }

        if (gallopCount == 0) {
            gallopCount = gallopThreshold;

            if (i < len1 && j < len2 && left[i] > right[j]) {
                int skip = gallopRight(left, right[j], i, len1 - i, 0);
                for (int x = i; x < i + skip; x++) {
                    arr[k++] = left[x];
                }
                i += skip;
                gallopCount = gallopThreshold;
            }

            if (j < len2 && i < len1 && right[j] >= left[i]) {
                int skip = gallopRight(right, left[i], j, len2 - j, 0);
                for (int x = j; x < j + skip; x++) {
                    arr[k++] = right[x];
                }
                j += skip;
                gallopCount = gallopThreshold;
            }
        }
    }

    while (i < len1) arr[k++] = left[i++];
    while (j < len2) arr[k++] = right[j++];
}

void timSort(std::vector<int>& arr) {
    int n = arr.size();
    if (n < 2) return;

    int minRun = calcMinRun(n);

    for (int i = 0; i < n; i += minRun) {
        insertionSort(arr, i, std::min(i + minRun - 1, n - 1));
    }

    for (int size = minRun; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = std::min(left + 2 * size - 1, n - 1);

            if (mid < right) {
                if (size > MIN_MERGE) {
                    mergeWithGallop(arr, left, mid, right);
                }
                else {
                    merge(arr, left, mid, right);
                }
            }
        }
    }
}


int main() {
    vector<int> arr = {
    -342, 876, -123, 567, -890, 234, -678, 901, -456, 789,
    123, -987, 654, -321, 888, -555, 222, -777, 333, -666,
    444, -888, 999, -111, 777, -222, 555, -333, 666, -444,
    -789, 345, -912, 678, -234, 890, -567, 123, -876, 456,
    911, -278, 634, -490, 827, -163, 598, -725, 381, -942,
    257, -819, 463, -190, 728, -354, 695, -281, 937, -406,
    582, -739, 164, -895, 427, -618, 293, -746, 159, -832,
    674, -297, 841, -530, 968, -175, 612, -387, 924, -651,
    738, -294, 861, -527, 198, -763, 425, -698, 351, -814,
    672, -239, 885, -516, 743, -182, 629, -475, 916, -603,
    -147, 792, -365, 818, -253, 697, -431, 964, -578, 125,
    -862, 409, -736, 281, -915, 647, -392, 873, -519, 236,
    -784, 351, -628, 497, -863, 214, -759, 382, -946, 571,
    -208, 835, -461, 728, -193, 654, -827, 319, -785, 462,
    -139, 896, -573, 241, -908, 675, -324, 987, -651, 238,
    -794, 367, -812, 495, -736, 289, -954, 617, -483, 172,
    -829, 456, -713, 398, -265, 841, -579, 324, -896, 657,
    -234, 789, -412, 963, -578, 145, -821, 496, -763, 328,
    -915, 672, -389, 814, -257, 931, -648, 275, -793, 416,
    -582, 739, -164, 895, -427, 618, -293, 746, -159, 832
    };

    cout << "Исходный массив: ";
    for (int num : arr) cout << num << " ";
    cout << endl;

    timSort(arr);

    cout << "Отсортированный массив: ";
    for (int num : arr) cout << num << " ";
    cout << endl;

    return 0;
}