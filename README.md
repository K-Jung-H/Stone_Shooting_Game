# Stone_Shooting_Game

진행 과정:

셰이더 객체에서 셰이더 리소스(재질 버퍼, 게임 객체 버퍼)를 관리하도록 함
+
렌더링 전, 필요 정보를 컨테이너에 담아서 셰이더로 전달하면, 셰이더가 해당하는 버퍼를 업데이트하여 바인딩

객체에 있는 재질 객체를 vector 컨테이너에서 관리하도록 변경
한 객체 또는 한 재질에 각각 다른 셰이더가 모두 적용 가능할 수 있도록 코드 변경 // 실제로 어떻게 활용할 수 있을지 고민 필요

==========핵심 목표==========
결론적으로, 시작 셰이더를 함수의 인자로 전달하여, 그리기를 진행하되, 
도중, 다른 셰이더를 사용해야 하는 상황(재질이 별도의 셰이더를 갖고 있다면)이 발생하면,
해당 셰이더로 특정 객체를 렌더링 한 후, 인자로 전달된 기존 셰이더로 돌아와 다시 렌더링 동작하게 하는 중

Render의 인자로 shader을 입력 받아서, 객체의 재질에 관하여 새로운 shader이 적용되더라도, 
다음 객체를 그릴 때는 다시 초기 shader로 설정되도록 하여 해결 중

 
===========추가 목표=========

추가적으로 셰이더 타입 별로 enum 집합을 만들어서, 구별하기 편하게 만들어보기
구별해서 PSO전환 유무를 판별하면 속도 향상될 듯

프레임워크 뜯어고치면, 재질 정보를 저장해놓는 cpp 따로 만들기 -> 가독성

PSO를 전역으로 작성하여, 필요시 적용하고 싶음.

==================================================


# 보드의 격자무늬를 어떻게 표현할 지 고민 필요  
-> 다음 할 내용 객체 계층 구조로 나타내기
--> 보드, 보드의 격자 무늬를 계층 구조로 표현하기

추가할 내용 

물리적 요소:
물리연산에 질량 요소 추가해서 하기 // 마찰력 말고도 질량을 별도로 추가

필드 요소:
필드 다양성 추가해보기

아이템 요소:
보드에 아이템 배치 및
추가적인 키 입력으로 스킬

아이템 종류:
- 슈팅 파워를 반으로 하는 대신 2번 연속으로 플레이
- 상대를 통과해 지나가는 슈팅
- 커브
- 무조건 플파워


UI:
아이템 UI


패링 시스템 만들기 // 로아 레이드 참조 // 키패드로 타이밍 맟줘 누르기



