#!/bin/bash -
export  NumThreadLite=1
export NumClusterLite=1

function classify_cat()
{
    ./build/tests/tm_classfication -n $1
    if [ "$?" != 0 ]; then
        echo "failed"
        return 0
    else
        echo "pass"
        return 1
    fi
}

function classify_bike()
{
    ./build/tests/tm_classfication -n $1 -i ./images/bike.jpg
    if [ "$?" != 0 ]; then
        echo "failed"
        return 0
    else
        echo "pass"
        return 1
    fi
}


classify_models=(squeezenet mobilenet mobilenet_v2 alexnet googlenet inception_v3 inception_v4 resnet50 vgg16 mnasnet shufflenet_1xg3 shufflenet_v2 resnet18_v2_mx )

for i in ${classify_models[@]}
do
    if [ "$i" == "resnet50" ]; then
        classify_bike $i
    else
        classify_cat $i
    fi

    if [ "$?" == 0 ]; then
        failed_models=("${failed_models[@]}" "$i")
        failed_model_count=$((${failed_model_count} + 1))
    else
        pass_models=("${pass_models[@]}" "$i")
        pass_model_count=$((${pass_model_count} + 1))
    fi
    total_model_count=$((${total_model_count} + 1))
done

echo
echo "total model count:$total_model_count "
echo "pass model count:$pass_model_count "
echo "failed model count:$failed_model_count "
echo

echo "failed model list:"
for i in ${failed_models[@]}
do
    echo -n "$i " 
done

if [ "$failed_model_count" != 0 ]; then
exit 1
