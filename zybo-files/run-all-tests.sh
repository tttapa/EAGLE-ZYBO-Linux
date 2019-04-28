set -e

cd $1
. /media/ldpath.sh
for test in *.test; do
    ./$test --gtest_color=yes
done