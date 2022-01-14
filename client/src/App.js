import './App.css'

import React, { useEffect, useRef, useState } from 'react'
// import useChat from './useChat'
import { Upload, Button, Input, message, Tag } from 'antd'
import { UploadOutlined } from '@ant-design/icons'
const fs = require('fs');

function App() {
  // const { status, opened, messages, sendMessage, clearMessages } = useChat()
  
  // const http = new XMLHttpRequest()
  // http.open("POST", "https://localhost:4000", true)
  // http.setRequestHeader('Content-Type', 'text/plain;charset=UTF-8')
  var http = require('http');
  // var agent = new http.Agent({ keepAlive: true }); // false by default
  
  

  // http.request({
  //     host: '127.0.0.1',
  //     port: 4000,
  //     method: 'POST',
  //     agent: agent, // use this agent for more requests as needed
  //     data: 'lll'
  // }).end();
  const [id, setID] = useState(0)
  const [messages, setMessages] = useState("")
  const [messagesList, setMessagesList] = useState([])
  const [friends, setFriends] = useState([])
  const [username, setUsername] = useState('')
  const [options, setOptions] = useState(false)
  const [chatRoom, setChatRoom] = useState(false)
  const [chatting, setChatting] = useState(false)
  const [chattingFriend, setChattingFriend] = useState("")
  const [addFriend, setAddFriend] = useState(false)
  const [removeFriend, setRemoveFriend] = useState(false)
  const [listFriends, setListFriends] = useState(false)

  const bodyRef = useRef(null)

  const MAX_SIZE_OF_DATA = 3000;
  const SERVER_BUF_SIZE = 4096;

  const props = {
    name: 'file',
    action: 'https://www.mocky.io/v2/5cc8019d300000980a055e76',
    headers: {
      authorization: 'authorization-text',
    }
  }

  let friendString = ""
  let messageString = ""
  let fileString = ""

  useEffect(() => {
    if(chatting){
      bodyRef.current.scrollTop = bodyRef.current.scrollHeight
    }
  })

  const displayStatus = (s) => {
    if (s.msg) {
      const { type, msg } = s
      const content = {
        content: msg,
        duration: 0.5
      }

      switch (type) {
        case 'success':
          message.success(content)
          break
        case 'info':
          message.info(content)
          break
        case 'danger':
        default:
          message.error(content)
          break
      }
    }
  }

  return (
    <div className="App">
      {!options ?
        <div> 
          <div className="App-title">
            <h1>Login</h1>
          </div>
          <Input.Search
            placeholder="Please enter a username:"
            enterButton="Continue"
            // ref={bodyRef}
            style={{ marginBottom: 10 }}
            onChange={(e) => setUsername(e.target.value)}
            onSearch={(msg) => {
              if(msg) {
                  ////////////////////////// sendMessage(`Login {username}`)
                  const da = `Login ${msg}`
                  const option = {
                    hostname: '127.0.0.1',
                    port: 4000,
                    method: 'POST',
                    headers: {
                      'Content-Type': 'text/plain',
                      'Content-Length': da.length,
                    }
                  }
                  const req = http.request(option, res => {
                    // console.log(res)
                    // console.log(`statusCode: ${res.statusCode}`)
                    res.on('data', d => {
                      // console.log(d)
                      setID(d);
                    })
                  })
                  // console.log(req)

                  req.on('error', error => {
                    console.error(error)
                  })

                  req.write(da)
                  req.end()
                  displayStatus({
                    type: 'success',
                    msg: 'Logged in sucessfully'
                  })
                  setOptions(true)
              }
              else {
                displayStatus({
                  type: 'error',
                  msg: 'Please enter a username.'
                })
              } 
            }}>
            </Input.Search>
        </div> 
        : 
        <>
        {listFriends ?
          <>
            <div className="App-title">
              <h1>All friends</h1>
            </div>
            <div>
              {friends.map((e, i) => (
                <p className="App-message" key={i}>
                  <Tag color="blue">{e}</Tag>
                </p>
              ))}
            </div>
            <Button type="primary" danger onClick={() => {
              setListFriends(false)
            }}>
              Leave
            </Button>
          </>
          : 
          <>
            {addFriend ? 
              <>
                <div className="App-title">
                  <h1>Add friends</h1>
                </div>
                <Input.Search
                  placeholder="Please enter a username:"
                  enterButton="Add"
                  // ref={bodyRef}
                  style={{ marginBottom: 10 }}
                  onChange={(e) => setUsername(e.target.value)}
                  onSearch={(msg) => {
                    if(msg) {
                        ////////////////////// sendMessage(`Add {username}`)
                        const da = `Add ${msg} ${id}`
                        const option = {
                          hostname: '127.0.0.1',
                          port: 4000,
                          method: 'POST',
                          headers: {
                            'Content-Type': 'text/plain',
                            'Content-Length': da.length
                          }
                        }

                        const req = http.request(option, res => {
                          // console.log(`statusCode: ${res.statusCode}`)
                          res.on('data', d => {
                            // console.log(d)
                            if(d[0] === 49){
                              displayStatus({
                                type: 'error',
                                msg: 'No user found.'
                              })
                            }
                            else if(d[0] === 48){
                              displayStatus({
                                type: 'success',
                                msg: 'Added successfully.'
                              })
                            }
                          })
                        })

                        req.on('error', error => {
                          console.error(error)
                        })

                        req.write(da)
                        req.end()
                    }
                    else {
                      displayStatus({
                        type: 'error',
                        msg: 'Please enter a username.'
                      })
                    } 
                  }}>
                </Input.Search>
                <Button type="primary" danger onClick={() => {
                  setAddFriend(false)
                }}>
                  Leave
                </Button>
              </>
              :
              <>
                {removeFriend ? 
                  <>
                    <div className="App-title">
                      <h1>Remove friends</h1>
                    </div>
                    <div>
                      {friends.map((e, i) => (
                        <p className="App-message" key={i}>
                          <Button type="ghost" onClick={() => {
                            const da = `Remove ${e} ${id}`
                            const option = {
                              hostname: '127.0.0.1',
                              port: 4000,
                              method: 'POST',
                              headers: {
                                'Content-Type': 'text/plain',
                                'Content-Length': da.length
                              }
                            }
    
                            const req = http.request(option, res => {
                              // console.log(`statusCode: ${res.statusCode}`)
                              res.on('data', d => {
                                // console.log(d)
                                displayStatus({
                                  type: 'success',
                                  msg: 'Removed successfully.'
                                })
                                const index = friends.indexOf(e);
                                const copy = [...friends]
                                copy.splice(index, 1)
                                setFriends(copy)
                              })
                            })
    
                            req.on('error', error => {
                              console.error(error)
                            })
    
                            req.write(da)
                            req.end()
                          }}>{e}</Button>
                        </p>
                      ))}
                    </div>
                    <Button type="primary" danger onClick={() => {
                      setRemoveFriend(false)
                    }}>
                      Leave
                    </Button>
                  </>
                  :
                  <>
                    {chatRoom ? 
                      <>
                        <div className="App-title">
                          <h1>Chat room</h1>
                        </div>
                        <div>
                          {friends.map((e, i) => (
                            <p className="App-message" key={i}>
                              <Button type="ghost" onClick={() => {
                                const da = `Chat ${e} ${id}`
                                const option = {
                                  hostname: '127.0.0.1',
                                  port: 4000,
                                  method: 'POST',
                                  headers: {
                                    'Content-Type': 'text/plain',
                                    'Content-Length': da.length
                                  }
                                }
        
                                const req = http.request(option, res => {
                                  // console.log(`statusCode: ${res.statusCode}`)
                                  res.on('data', d => {
                                    // console.log(d)
                                    messageString = ""
                                    for(var i=0; i<d.length; i++){
                                      messageString += String.fromCharCode(d[i])
                                    }
                                    messageString = messageString.slice(0,-1)
                                    // console.log(messageString)
                                    const copy = messageString.split('\n')
                                    copy.splice(0, 1)
                                    // console.log(copy)
                                    setMessagesList(copy)
                                  })
                                })
          
                                req.on('error', error => {
                                  console.error(error)
                                })
        
                                req.write(da)
                                req.end()
                                setChattingFriend(e)
                                setChatting(true)
                                setChatRoom(false)
                              }}>{e}</Button>
                            </p>
                          ))}
                        </div>

                        <Button type="primary" danger onClick={() => {
                          setChatRoom(false)
                        }}>Leave</Button>
                      </>
                      :
                      <>
                        {chatting ? 
                          <>
                            <div className="App-title">
                              <h1>{chattingFriend}</h1>
                            </div>
                            <div className="App-messages" ref={bodyRef}>
                              {messagesList.map((e, i) => (
                                e[0] === 'A' ? (
                                  <p key={i} align="right">
                                    <Tag color="#69c0ff">{e.slice(3)}</Tag>
                                  </p> 
                                ) : ( e[0] === 'B' ? (
                                    <p key={i} align="left">
                                      <Tag color="#95de64">{e.slice(3)}</Tag>
                                    </p>
                                  ) : ( e[1] === 'A' ? (
                                    <p key={i} align="right">
                                      <Tag color="#096dd9" onClick={() => {
                                        let da = `Download${id} ${e.slice(4)}`
                                        let option = {
                                          hostname: '127.0.0.1',
                                          port: 4000,
                                          method: 'POST',
                                          headers: {
                                            'Content-Type': 'text/plain',
                                            'Content-Length': da.length
                                          }
                                        }
                                        let req = http.request(option, res => {
                                          // console.log(`statusCode: ${res.statusCode}`)
                                          res.on('data', d => {
                                            console.log(d)
                                          })
                                        })
                                        req.on('error', error => {
                                          console.error(error)
                                        })
                                        req.write(da)
                                        req.end()

                                        while(true){
                                          da = `DownloadImme${id}`
                                          // da += reader.result[i]
                                          // i++; j++
                                          option = {
                                            hostname: '127.0.0.1',
                                            port: 4000,
                                            method: 'POST',
                                            headers: {
                                              'Content-Type': 'text/plain',
                                              'Content-Length': da.length
                                            }
                                          }
                                          req = http.request(option, res => {
                                            // console.log(`statusCode: ${res.statusCode}`)
                                            res.on('data', d => {
                                              // console.log(d)
                                              fileString = ""
                                              for(var i=0; i<d.length; i++){
                                                fileString += String.fromCharCode(d[i])
                                              }
                                              console.log(fileString)
                                              console.log(fileString.length)

                                              const content = 'Some content!'

                                              fs.writeFile('test.txt', content, err => {
                                                if (err) {
                                                  console.error(err)
                                                  return
                                                }
                                                //file written successfully
                                              })

                                            })
                                          })
                                          req.on('error', error => {
                                            console.error(error)
                                          })
                                          req.write(da);
                                          req.end()
                                          // j = 0
                                          // da = ""
                                          break
                                        }
                                      }}>{e.slice(4)}</Tag>
                                    </p> 
                                    ) : (
                                      <p key={i} align="left">
                                        <Tag color="#389e0d" onClick={() => {
                                          console.log("Here")
                                          let da = `Download${id} ${e.slice(4)}`
                                          let option = {
                                            hostname: '127.0.0.1',
                                            port: 4000,
                                            method: 'POST',
                                            headers: {
                                              'Content-Type': 'text/plain',
                                              'Content-Length': da.length
                                            }
                                          }
                                          let req = http.request(option, res => {
                                            // console.log(`statusCode: ${res.statusCode}`)
                                            res.on('data', d => {
                                              // console.log(d)
                                            })
                                          })
                                          req.on('error', error => {
                                            console.error(error)
                                          })
                                          req.write(da)
                                          req.end()

                                          while(true){
                                            da = `DownloadImme${id}`
                                            // da += reader.result[i]
                                            // i++; j++
                                            option = {
                                              hostname: '127.0.0.1',
                                              port: 4000,
                                              method: 'POST',
                                              headers: {
                                                'Content-Type': 'text/plain',
                                                'Content-Length': da.length
                                              }
                                            }
                                            req = http.request(option, res => {
                                              // console.log(`statusCode: ${res.statusCode}`)
                                              res.on('data', d => {
                                                console.log(d)
                                              })
                                            })
                                            req.on('error', error => {
                                              console.error(error)
                                            })
                                            req.write(da);
                                            req.end()
                                            // j = 0
                                            // da = ""
                                            break
                                          }
                                        }}>{e.slice(4)}</Tag>
                                      </p> 
                                    )
                                  )
                                )
                              ))}
                            </div>
                            <Input.Search
                              // rows={4}
                              value={messages}
                              // ref={bodyRef}
                              enterButton="Send"
                              onChange={(e) => setMessages(e.target.value)}
                              placeholder="Type a message here..."
                              onSearch={(msg) => {
                                if (!msg) {
                                  displayStatus({
                                    type: 'error',
                                    msg: 'Please enter a message.'
                                  })
                                  return
                                }
                                else {
                                  const da = `Text${id} ${msg}`
                                  const option = {
                                    hostname: '127.0.0.1',
                                    port: 4000,
                                    method: 'POST',
                                    headers: {
                                      'Content-Type': 'text/plain',
                                      'Content-Length': da.length
                                    }
                                  }
          
                                  const req = http.request(option, res => {
                                    // console.log(`statusCode: ${res.statusCode}`)
                                    res.on('data', d => {
                                      // console.log(d)
                                    })
                                  })
            
                                  req.on('error', error => {
                                    console.error(error)
                                  })
          
                                  req.write(da)
                                  req.end()
                                  msg = "A: " + msg
                                  setMessagesList([...messagesList, msg])
                                }
                                setMessages('')
                              }}
                            ></Input.Search>
                            <br/>
                            <Upload {...props} showUploadList={false} beforeUpload={(file) => {
                              const reader = new FileReader();

                              reader.onload = () => {
                                // console.log(reader.result)
                                let da = `FileName${id} ${file.name}`
                                let option = {
                                  hostname: '127.0.0.1',
                                  port: 4000,
                                  method: 'POST',
                                  headers: {
                                    'Content-Type': 'text/plain',
                                    'Content-Length': da.length
                                  }
                                }
                                let req = http.request(option, res => {
                                  // console.log(`statusCode: ${res.statusCode}`)
                                  res.on('data', d => {
                                    // console.log(d)
                                  })
                                })
                                req.on('error', error => {
                                  console.error(error)
                                })
                                req.write(da)
                                req.end()
                                

                                let i = 0, j = 0;
                                da = ""
                                console.log(reader.result.length)
                                while(i < reader.result.length){
                                  da += reader.result[i]
                                  i++; j++
                                  if(j === MAX_SIZE_OF_DATA - id.length - 9){
                                    option = {
                                      hostname: '127.0.0.1',
                                      port: 4000,
                                      method: 'POST',
                                      headers: {
                                        'Content-Type': 'text/plain',
                                        'Content-Length': da.length
                                      }
                                    }
                                    req = http.request(option, res => {
                                      // console.log(`statusCode: ${res.statusCode}`)
                                      res.on('data', d => {
                                        // console.log(d)
                                      })
                                    })
                                    req.on('error', error => {
                                      console.error(error)
                                    })
                                    req.write(`FileImme${id} ${da}`);
                                    console.log(da)
                                    req.end()
                                    j = 0
                                    da = ""
                                  }
                                }


                                console.log(da.length)

                                let len = da.length
                                da += '\0'.repeat(MAX_SIZE_OF_DATA - da.length - 12 - id.length - `${da.length}`.length)
                                da = `FileFinish${id} ${len} ${da}`
                                option = {
                                  hostname: '127.0.0.1',
                                  port: 4000,
                                  method: 'POST',
                                  headers: {
                                    'Content-Type': 'text/plain',
                                    'Content-Length': da.length
                                  }
                                }
                                req = http.request(option, res => {
                                  // console.log(`statusCode: ${res.statusCode}`)
                                  res.on('data', d => {
                                    // console.log(d)
                                  })
                                })
                                req.on('error', error => {
                                  console.error(error)
                                })
                                req.write(da)
                                displayStatus({
                                  type: 'success',
                                  msg: 'Uploaded successfully.'
                                })
                                req.end()
                              };
                              reader.readAsBinaryString(file);
                      
                              // Prevent upload
                              return false;
                            }}>
                              <Button icon={<UploadOutlined />}>Upload a file or an image</Button>
                            </Upload>
                            <br/>
                            <Button type="primary" danger onClick={() => {
                              setChatting(false)
                              setChatRoom(true)
                            }}>Leave</Button>
                          </>
                          :
                          <>
                            <div className="App-title">
                              <h1>Welcome, {username}!</h1>
                            </div>
                            <div className="App-title">
                              <h2>Choose an option to continue...</h2>
                            </div>
                            <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
                              /////////////////////sendMessage('List friends')
                              // http.onload = () => console.log(http.responseText)
                              // http.send('List friends')
                              //////////////////////////
                              // const da = JSON.stringify({
                              //   todo: 'Buy the milk'
                              // })
                              const da = "List friends"
                              const option = {
                                hostname: '127.0.0.1',
                                port: 4000,
                                method: 'POST',
                                headers: {
                                  'Content-Type': 'text/plain',
                                  'Content-Length': da.length,
                                }
                              }

                              const req = http.request(option, res => {
                                // console.log(`statusCode: ${res.statusCode}`)
                                res.on('data', d => {
                                  // console.log(d)
                                  friendString = ""
                                  for(var i=0; i<d.length; i++){
                                    friendString += String.fromCharCode(d[i])
                                  }
                                  friendString = friendString.slice(0,-1)
                                  // console.log(friendString)
                                  setFriends(friendString.split('\n'))
                                })
                              })

                              req.on('error', error => {
                                console.error(error)
                              })

                              req.write(`List friends ${id}`)
                              req.end()
                              /////////////////////////////
                              setListFriends(true)
                            }}>
                              List all friends
                            </Button>
                            <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
                              setAddFriend(true)
                            }}>
                              Add friends
                            </Button>
                            <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
                              const da = "List friends"
                              const option = {
                                hostname: '127.0.0.1',
                                port: 4000,
                                method: 'POST',
                                headers: {
                                  'Content-Type': 'text/plain',
                                  'Content-Length': da.length,
                                }
                              }

                              const req = http.request(option, res => {
                                // console.log(`statusCode: ${res.statusCode}`)
                                res.on('data', d => {
                                  // console.log(d)
                                  friendString = ""
                                  for(var i=0; i<d.length; i++){
                                    friendString += String.fromCharCode(d[i])
                                  }
                                  friendString = friendString.slice(0,-1)
                                  // console.log(friendString)
                                  setFriends(friendString.split('\n'))
                                })
                              })

                              req.on('error', error => {
                                console.error(error)
                              })

                              req.write(`List friends ${id}`)
                              req.end()
                              setRemoveFriend(true)
                            }}>
                              Remove friends
                            </Button>
                            <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
                              const da = "List friends"
                              const option = {
                                hostname: '127.0.0.1',
                                port: 4000,
                                method: 'POST',
                                headers: {
                                  'Content-Type': 'text/plain',
                                  'Content-Length': da.length,
                                }
                              }

                              const req = http.request(option, res => {
                                // console.log(`statusCode: ${res.statusCode}`)
                                res.on('data', d => {
                                  // console.log(d)
                                  friendString = ""
                                  for(var i=0; i<d.length; i++){
                                    friendString += String.fromCharCode(d[i])
                                  }
                                  friendString = friendString.slice(0,-1)
                                  // console.log(friendString)
                                  setFriends(friendString.split('\n'))
                                })
                              })

                              req.on('error', error => {
                                console.error(error)
                              })

                              req.write(`List friends ${id}`)
                              req.end()
                              setChatRoom(true)
                            }}>
                              Chat with
                            </Button>
                          </>
                        }
                      </>
                    }
                  </>
                }
              </>
            }
          </>
        }
        </>
      }
    </div>
  )
}

export default App
