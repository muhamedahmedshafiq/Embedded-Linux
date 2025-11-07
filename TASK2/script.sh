#!/bin/bash
mkdir myDirectory/secondDirectory -p
cd myDirectory/secondDirectory
touch myNotePaper
mv myNotePaper ../
cd ..
mv myNotePaper myOldNotePaper
